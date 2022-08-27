#pragma once

#include "wait_queue.h"
#include <types.h>

class Semaphore
{
  public:
	NON_COPYABLE(Semaphore)
	explicit Semaphore(int initial_value);
	Semaphore(Semaphore&&);
	Semaphore& operator=(Semaphore&&);
	~Semaphore();
	void acquire();
	void release();

  private:
	UniquePointer<Spinlock> m_lock;
	int m_count;
	WaitQueue m_queue;
	bool is_moved = false;
};
