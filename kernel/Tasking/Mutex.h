#pragma once
#include "Semaphore.h"

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