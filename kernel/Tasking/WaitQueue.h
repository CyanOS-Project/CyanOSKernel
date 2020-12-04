#pragma once
#include "ScopedLock.h"
#include "SpinLock.h"
#include "Thread.h"
#include <IntrusiveList.h>
#include <Rule5.h>

class WaitQueue
{
  public:
	NON_COPYABLE(WaitQueue);
	NON_MOVABLE(WaitQueue);

	WaitQueue();
	~WaitQueue();

	void terminate_blocked_thread(Thread&);
	void wake_up(size_t num = 1);
	void wake_up_all();

	template <typename T> void wait(ScopedLock<T>& checker_lock)
	{
		ScopedLock queue_lock(m_lock);
		Thread::current->block(*this);
		m_threads.push_back(*Thread::current);
		queue_lock.release();
		checker_lock.release();

		Thread::yield();
		checker_lock.acquire();
	}
	template <typename T, typename L> void wait_on_event(T checker, ScopedLock<L>& checker_lock)
	{
		ScopedLock queue_lock(m_lock);

		while (checker()) {
			Thread::current->block(*this);
			m_threads.push_back(*Thread::current);
			queue_lock.release();
			checker_lock.release();

			Thread::yield();

			checker_lock.acquire();
			queue_lock.acquire();
		}
	}

  private:
	Spinlock m_lock;
	IntrusiveList<Thread> m_threads;
	void wake_up_one();
};
