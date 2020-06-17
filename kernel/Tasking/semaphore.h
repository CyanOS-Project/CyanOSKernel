#include "scheduler.h"
#include "utils/types.h"

struct Semaphore {
	unsigned max_count;
	unsigned count;
	SpinLock spinlock;
	ThreadControlBlock* waiting_queue;
};

void semaphore_init(Semaphore* lock);
void semaphore_acquire(Semaphore* lock);
void semaphore_release(Semaphore* lock);