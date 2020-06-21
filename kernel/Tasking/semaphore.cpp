#include "semaphore.h"
#include "Lib/stdlib.h"

void semaphore_init(Semaphore* lock)
{
	lock->count = 0;
	lock->max_count = 1;
	lock->waiting_queue = new CircularList<ThreadControlBlock>;
	spinlock_init(&lock->spinlock);
}

void semaphore_acquire(Semaphore* lock)
{
	spinlock_acquire(&lock->spinlock);
	if (lock->count < lock->max_count) {
		lock->count++;
		spinlock_release(&lock->spinlock);
	} else {
		Scheduler::block_current_thread(ThreadState::BLOCKED_LOCK, lock->waiting_queue);
		spinlock_release(&lock->spinlock);
		Scheduler::yield();
		semaphore_acquire(lock);
	}
}

void semaphore_release(Semaphore* lock)
{
	spinlock_acquire(&lock->spinlock);
	if (!lock->waiting_queue->is_empty()) {
		Scheduler::unblock_thread(lock->waiting_queue);
		if (!lock->waiting_queue->is_empty())
			lock->waiting_queue->increment_head();
	}
	lock->count--;
	spinlock_release(&lock->spinlock);
}
