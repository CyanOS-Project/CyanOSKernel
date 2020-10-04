#pragma once

#include "WaitQueue.h"
#include <Types.h>

class Semaphore
{
  public:
	explicit Semaphore(size_t t_max_count, size_t t_initial_value = 0);
	~Semaphore();
	void acquire();
	void release();

  private:
	Spinlock m_lock;
	unsigned m_max_count;
	unsigned m_count;
	WaitQueue m_queue;
};
