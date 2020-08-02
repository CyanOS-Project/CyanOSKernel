#include "Thread.h"

CircularQueue<Thread>* Thread::ready_threads = nullptr;
CircularQueue<Thread>* Thread::sleeping_threads = nullptr;
Thread* Thread::current = nullptr;
Bitmap* Thread::m_tid_bitmap;

void Thread::setup()
{
	m_tid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	ready_threads = new CircularQueue<Thread>;
	sleeping_threads = new CircularQueue<Thread>;
}

void Thread::create_thread(const Process& parent_process, thread_function address, uintptr_t argument)
{
	ready_threads->emplace_back(parent_process, address, argument);
}

Thread::Thread(const Process& parent_process, thread_function address, uintptr_t argument) : parent{parent_process}
{
	spinlock_init(&m_lock);
	void* thread_kernel_stack = Memory::alloc(STACK_SIZE, MEMORY_TYPE::WRITABLE | MEMORY_TYPE::KERNEL);

	uintptr_t stack_pointer = Context::setup_task_stack_context(thread_kernel_stack, STACK_SIZE, uintptr_t(address), //
	                                                            uintptr_t(idle), argument);
	tid = reserve_tid();
	kernel_stack_start = uintptr_t(thread_kernel_stack);
	kernel_stack_end = kernel_stack_start + STACK_SIZE;
	kernel_stack_pointer = stack_pointer;
	m_state = ThreadState::READY;
}

Thread::~Thread()
{
}

void Thread::block(ThreadState reason)
{
	UNUSED(reason);
	spinlock_acquire(&m_lock);
	Thread& current = ready_threads->head();
	current.m_state = ThreadState::BLOCKED_LOCK;
	// ready_threads->move_head_to_other_list(waiting_list);FIXME:
	spinlock_release(&m_lock);
}

void Thread::resume()
{
	spinlock_acquire(&m_lock);
	// waiting_list->move_head_to_other_list(ready_threads);FIXME:
	spinlock_release(&m_lock);
}

// schedule another thread.
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
	/*for (auto&& thr = ready_threads->begin(); thr != ready_threads->end(); ++thr) {
	    if (thr->tid == thread->tid) {
	        ready_threads->remove(thr);
	    }
	}*/
	spinlock_release(&m_lock);
}

void Thread::sleep(unsigned ms)
{
	spinlock_acquire(&current->m_lock);
	current->sleep_ticks = PIT::ticks + ms;
	current->m_state = ThreadState::BLOCKED_SLEEP;
	ready_threads->move_head_to_other_list(sleeping_threads);
	spinlock_release(&current->m_lock);
	yield();
}