#pragma once

#include "Lib/stdlib.h"
#include "scheduler.h"
#include "utils/list.h"
#include "utils/types.h"

class Semaphore
{
  private:
	unsigned m_max_count;
	unsigned m_count;
	SpinLock m_spinlock;
	CircularQueue<ThreadControlBlock>* m_waiting_queue;

  public:
	Semaphore(int t_max_count);
	~Semaphore();
	void acquire();
	void release();
};
