#include "Thread.h"
#include "WaitQueue.h"

IntrusiveList<Thread>* Thread::ready_threads = nullptr;
IntrusiveList<Thread>* Thread::sleeping_threads = nullptr;
Thread* Thread::current = nullptr;
Bitmap* Thread::m_tid_bitmap;

void Thread::setup()
{
	m_tid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	ready_threads = new IntrusiveList<Thread>;
	sleeping_threads = new IntrusiveList<Thread>;
}

Thread& Thread::create_thread(Process& parent_process, thread_function address, uintptr_t argument)
{
	Thread& new_thread = *new Thread(parent_process, address, argument);
	ready_threads->push_back(new_thread);
	return new_thread;
}

Thread::Thread(Process& parent_process, thread_function address, uintptr_t argument) :
    m_tid{reserve_tid()},
    m_parent{parent_process}
{
	spinlock_init(&m_lock);
	void* thread_kernel_stack = Memory::alloc(STACK_SIZE, MEMORY_TYPE::WRITABLE | MEMORY_TYPE::KERNEL);

	uintptr_t stack_pointer = Context::setup_task_stack_context(thread_kernel_stack, STACK_SIZE, uintptr_t(address), //
	                                                            uintptr_t(idle), argument);
	m_kernel_stack_start = uintptr_t(thread_kernel_stack);
	m_kernel_stack_end = m_kernel_stack_start + STACK_SIZE;
	m_kernel_stack_pointer = stack_pointer;
	m_state = ThreadState::RUNNABLE;
}

Thread::~Thread()
{
}

void Thread::wake_up_from_queue()
{
	spinlock_acquire(&m_lock);
	ready_threads->push_back(*this);
	m_state = ThreadState::RUNNABLE;
	spinlock_release(&m_lock);
}

void Thread::wake_up_from_sleep()
{
	spinlock_acquire(&m_lock);
	sleeping_threads->remove(*this);
	ready_threads->push_back(*this);
	m_state = ThreadState::RUNNABLE;
	spinlock_release(&m_lock);
}

void Thread::wait_on(WaitQueue& queue)
{
	spinlock_acquire(&m_lock);
	ready_threads->remove(*this);
	queue.enqueue(*this);
	m_state = ThreadState::BLOCKED_QUEUE;
	spinlock_release(&m_lock);
	yield();
}

void Thread::yield()
{
	asm volatile("int 0x81");
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
	spinlock_acquire(&m_lock);
	ASSERT(m_state == ThreadState::RUNNABLE);
	this->~Thread();
}

void Thread::sleep(unsigned ms)
{
	spinlock_acquire(&current->m_lock);
	current->m_sleep_ticks = PIT::ticks + ms;
	current->m_state = ThreadState::BLOCKED_SLEEP;
	ready_threads->remove(*current);
	sleeping_threads->push_back(*current);
	spinlock_release(&current->m_lock);
	yield();
}