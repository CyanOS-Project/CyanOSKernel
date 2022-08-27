#pragma once
#include "semaphore.h"

class Mutex
{
  public:
	Mutex();
	~Mutex();
	void acquire();
	void release();

  private:
	Semaphore m_lock;
};