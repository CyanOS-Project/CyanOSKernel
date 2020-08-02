#pragma once

#include "asm.h"
#include "utils/types.h"

typedef struct spinlock_t {
	uint32_t value;
	uint32_t eflags;
} SpinLock;

void spinlock_init(SpinLock* lock);
void spinlock_acquire(SpinLock* lock);
void spinlock_release(SpinLock* lock);
