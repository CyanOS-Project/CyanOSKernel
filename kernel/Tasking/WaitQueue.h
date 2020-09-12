#pragma once
#include "IntrusiveList.h"
#include "Thread.h"

class WaitQueue
{
  public:
	WaitQueue();
	void wait();
	template <typename T, typename L> void wait_on_event(T checker, ScopedLock<L>& checker_lock)
	{
		ScopedLock queue_lock(m_lock);

		while (checker()) {
			Thread::current->block();
			m_threads.push_back(*Thread::current);
			queue_lock.release();
			checker_lock.release();

			Thread::yield();

			checker_lock.acquire();
			queue_lock.acquire();
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
