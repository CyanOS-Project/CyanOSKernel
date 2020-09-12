#pragma once

#include "Types.h"
#include "WaitQueue.h"

class Semaphore
{
  private:
	Spinlock m_lock;
	unsigned m_max_count;
	unsigned m_count;
	WaitQueue m_queue;

  public:
	explicit Semaphore(size_t t_max_count, size_t t_initial_value = 0);
	~Semaphore();
	void acquire();
	void release();
};
