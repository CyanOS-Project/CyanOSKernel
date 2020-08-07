#pragma once

#include "Arch/x86/spinlock.h"
#include "WaitQueue.h"
#include "utils/types.h"

class Semaphore
{
  private:
	unsigned m_max_count;
	unsigned m_count;
	SpinLock m_spinlock;
	WaitQueue m_queue;

  public:
	explicit Semaphore(size_t t_max_count, size_t t_initial_value = 0);
	~Semaphore();
	void acquire();
	void release();
};
