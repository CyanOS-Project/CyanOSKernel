#pragma once

#include "Lib/stdlib.h"
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
	Semaphore(size_t t_max_count);
	~Semaphore();
	void acquire();
	void release();
};
