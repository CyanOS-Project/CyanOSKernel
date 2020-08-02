#include "Thread.h"
#include "WaitQueue.h"

CircularQueue<Thread*>* Thread::ready_threads = nullptr;
CircularQueue<Thread*>* Thread::sleeping_threads = nullptr;
Thread* Thread::current = nullptr;
Bitmap* Thread::m_tid_bitmap;

void Thread::setup()
{
	m_tid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	ready_threads = new CircularQueue<Thread*>;
	sleeping_threads = new CircularQueue<Thread*>;
}

void Thread::create_thread(Process& parent_process, thread_function address, uintptr_t argument)
{
	ready_threads->emplace_back(new Thread(parent_process, address, argument));
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
	m_state = ThreadState::READY;
}

Thread::~Thread()
{
}

void Thread::wake_up()
{
	spinlock_acquire(&m_lock);
	ready_threads->push_back(this);
	m_state = ThreadState::RUNNING;
	spinlock_release(&m_lock);
}

void Thread::wait_on(WaitQueue& queue)
{
	spinlock_acquire(&m_lock);
	// FIXME: moving from queues this way costs 2 heap allocation instead of zero !
	// FIXME: implement find method on list.
	auto&& this_thread = ready_threads->begin();
	for (; this_thread != ready_threads->end(); ++this_thread) {
		if ((*this_thread)->tid() == m_tid)
			break;
	}
	ready_threads->remove(this_thread);
	queue.enqueue(*this);
	m_state = ThreadState::BLOCKED_LOCK;
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
	current->m_sleep_ticks = PIT::ticks + ms;
	current->m_state = ThreadState::BLOCKED_SLEEP;
	ready_threads->move_head_to_other_list(sleeping_threads);
	spinlock_release(&current->m_lock);
	yield();
}