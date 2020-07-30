#include "spinlock.h"

void spinlock_init(SpinLock* lock)
{
	lock->value = 0;
}

void spinlock_acquire(SpinLock* lock)
{
	lock->eflags = eflags_read();
	DISABLE_INTERRUPTS();
	while (test_and_set(&lock->value) != 0)
		;
}

void spinlock_release(SpinLock* lock)
{
	lock->value = 0;
	eflags_write(lock->eflags);
}
