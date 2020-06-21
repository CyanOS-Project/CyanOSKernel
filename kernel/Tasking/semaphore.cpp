#include "semaphore.h"
#include "Lib/stdlib.h"

Semaphore::Semaphore(int _max_count)
{
	max_count = _max_count;
	count = 0;
	waiting_queue = new CircularList<ThreadControlBlock>;
	spinlock_init(&spinlock);
}

Semaphore::~Semaphore()
{
	delete waiting_queue;
}

void Semaphore::acquire()
{
	spinlock_acquire(&spinlock);
	if (count < max_count) {
		count++;
		spinlock_release(&spinlock);
	} else {
		Scheduler::block_current_thread(ThreadState::BLOCKED_LOCK, waiting_queue);
		spinlock_release(&spinlock);
		Scheduler::yield();
		acquire();
	}
}

void Semaphore::release()
{
	if (!count)
		return;
	spinlock_acquire(&spinlock);
	if (!waiting_queue->is_empty()) {
		Scheduler::unblock_thread(waiting_queue);
		if (!waiting_queue->is_empty())
			waiting_queue->increment_head();
	}
	count--;
	spinlock_release(&spinlock);
}
