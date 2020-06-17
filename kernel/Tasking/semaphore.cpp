#include "semaphore.h"

void semaphore_init(Semaphore* lock)
{
	lock->count = 0;
}

void semaphore_acquire(Semaphore* lock)
{
	spinlock_acquire(&lock->spinlock);
	if (lock->count < lock->max_count) {
		lock->count++;
	} else {
		Scheduler::append_to_thread_list(&lock->waiting_queue, Scheduler::current_thread);
		Scheduler::block_current_thread(ThreadState::BLOCKED_LOCK);
	}
	spinlock_release(&lock->spinlock);
}

void semaphore_release(Semaphore* lock)
{
	spinlock_acquire(&lock->spinlock);
	if (lock->waiting_queue) {
		Scheduler::delete_from_thread_list(&lock->waiting_queue, lock->waiting_queue);
		Scheduler::append_to_thread_list(&Scheduler::active_threads, lock->waiting_queue);
	} else {
		lock->count--;
	}

	spinlock_release(&lock->spinlock);
}
