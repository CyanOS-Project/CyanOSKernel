#pragma once
#include "Thread.h"
#include "Utils/IntrusiveList.h"

class WaitQueue
{
  public:
	WaitQueue();
	template <typename T> void wait_on_event(T checker)
	{
		ScopedLock local_lock(m_lock);
		if (checker()) {
			Thread::current->block();
			m_threads.push_back(*Thread::current);
			local_lock.release();
			Thread::yield();
		}
	}

	void wake_up(size_t num = 1);
	void wake_up_all();

	WaitQueue(WaitQueue&&) = delete;
	WaitQueue(const WaitQueue&) = delete;
	WaitQueue& operator=(WaitQueue&&) = delete;
	WaitQueue& operator=(const WaitQueue&) = delete;
	~WaitQueue();

  private:
	Spinlock m_lock;
	IntrusiveList<Thread> m_threads;
	void wake_up_one();
};
