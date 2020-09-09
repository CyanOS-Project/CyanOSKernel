#include "Thread.h"
#include "Arch/x86/Context.h"
#include "Devices/Timer/Pit.h"
#include "ScopedLock.h"
#include "Utils/Assert.h"
#include "VirtualMemory/Memory.h"
#include "WaitQueue.h"

IntrusiveList<Thread>* Thread::ready_threads = nullptr;
IntrusiveList<Thread>* Thread::sleeping_threads = nullptr;
Thread* Thread::current = nullptr;
Bitmap* Thread::m_tid_bitmap;
StaticSpinlock Thread::global_lock;

void Thread::setup()
{
	global_lock.init();
	m_tid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	ready_threads = new IntrusiveList<Thread>;
	sleeping_threads = new IntrusiveList<Thread>;
}

Thread& Thread::create_thread(Process& parent_process, thread_function address, uintptr_t argument)
{
	ScopedLock local_lock(global_lock);
	Thread& new_thread = *new Thread(parent_process, address, argument);
	ready_threads->push_back(new_thread);
	return new_thread;
}

Thread::Thread(Process& parent_process, thread_function address, uintptr_t argument) :
    m_lock{},
    m_tid{reserve_tid()},
    m_parent{parent_process}
{
	m_lock.init();
	void* thread_kernel_stack = Memory::alloc(STACK_SIZE, MEMORY_TYPE::WRITABLE | MEMORY_TYPE::KERNEL);

	uintptr_t stack_pointer = Context::setup_task_stack_context(thread_kernel_stack, STACK_SIZE, uintptr_t(address), //
	                                                            uintptr_t(idle), argument);
	m_kernel_stack_start = uintptr_t(thread_kernel_stack);
	m_kernel_stack_end = m_kernel_stack_start + STACK_SIZE;
	m_kernel_stack_pointer = stack_pointer;
	m_state = ThreadState::RUNNABLE;
}

Thread::~Thread() {}

void Thread::wake_up_from_queue()
{
	ScopedLock local_lock(m_lock);
	ready_threads->push_back(*this);
	m_state = ThreadState::RUNNABLE;
}

void Thread::wake_up_from_sleep()
{
	ScopedLock local_lock(m_lock);
	sleeping_threads->remove(*this);
	ready_threads->push_back(*this);
	m_state = ThreadState::RUNNABLE;
}

void Thread::block()
{
	ScopedLock local_lock(m_lock);
	ready_threads->remove(*this);
	m_state = ThreadState::BLOCKED_QUEUE;
}

void Thread::yield()
{
	asm volatile("int $0x81");
}

void Thread::idle(_UNUSED_PARAM(uintptr_t))
{
	while (1) {
		HLT();
	}
}

unsigned Thread::reserve_tid()
{
	unsigned id = m_tid_bitmap->find_first_unused();
	m_tid_bitmap->set_used(id);
	return id;
}

void Thread::terminate()
{
	ScopedLock local_lock(m_lock);
	ASSERT(m_state == ThreadState::RUNNABLE);
	ready_threads->remove(*this);
	this->~Thread();
}

void Thread::sleep(unsigned ms)
{
	{
		ScopedLock local_lock(current->m_lock);
		current->m_sleep_ticks = PIT::ticks + ms;
		current->m_state = ThreadState::BLOCKED_SLEEP;
		ready_threads->remove(*current);
		sleeping_threads->push_back(*current);
	}
	yield();
}

unsigned Thread::tid()
{
	return m_tid;
}

Process& Thread::parent_process()
{
	return m_parent;
}

ThreadState Thread::state()
{
	return m_state;
}

size_t Thread::number_of_ready_threads()
{
	return ready_threads->size();
}