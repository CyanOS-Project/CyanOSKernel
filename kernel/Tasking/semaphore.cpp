/*#include "semaphore.h"
#include "Lib/stdlib.h"

void semaphore_init(Semaphore* lock)
{
    lock->count = 0;
    lock->max_count = 1;
    lock->waiting_queue = nullptr;
    spinlock_init(&lock->spinlock);
}

void semaphore_acquire(Semaphore* lock)
{
    spinlock_acquire(&lock->spinlock);
    if (lock->count < lock->max_count) {
        lock->count++;
        spinlock_release(&lock->spinlock);
    } else {
        Scheduler::block_current_thread(ThreadState::BLOCKED_LOCK);
        Scheduler::append_to_thread_list(&lock->waiting_queue, Scheduler::current_thread);
        spinlock_release(&lock->spinlock);
        Scheduler::yield();
        semaphore_acquire(lock);
    }
}

void semaphore_release(Semaphore* lock)
{
    spinlock_acquire(&lock->spinlock);
    if (lock->waiting_queue) {
        // FIXME: get rid of temp hack.
        ThreadControlBlock* temp_queue = lock->waiting_queue;
        Scheduler::delete_from_thread_list(&lock->waiting_queue, lock->waiting_queue);
        Scheduler::append_to_thread_list(&Scheduler::ready_threads, temp_queue);
    }
    lock->count--;
    spinlock_release(&lock->spinlock);
}
*/