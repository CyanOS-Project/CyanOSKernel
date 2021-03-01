#pragma once
#include "ScopedLock.h"
#include "SpinLock.h"
#include "Thread.h"
#include <IntrusiveList.h>
#include <Reference.h>
#include <Rule5.h>
#include <UniquePointer.h>
#include <Vector.h>

class WaitQueue
{
  public:
	NON_COPYABLE(WaitQueue)

	WaitQueue();
	WaitQueue(WaitQueue&&);
	WaitQueue& operator=(WaitQueue&&);
	~WaitQueue();

	void terminate_blocked_thread(Thread&);
	void wake_up(size_t num = 1);
	void wake_up_all();
	template <typename T> void wait(ScopedLock<T>& checker_lock, size_t ms_timeout = 0)
	{
		ScopedLock queue_lock(*m_lock);
		Thread::current->block(*this, ms_timeout);
		m_threads.push_back(*Thread::current);
		queue_lock.release();
		checker_lock.release();

		Thread::yield();

		checker_lock.acquire();
	}
	template <typename T, typename L> void wait_on_event(T checker, ScopedLock<L>& checker_lock, size_t ms_timeout = 0)
	{
		ScopedLock queue_lock(*m_lock);

		while (checker()) {
			Thread::current->block(*this, ms_timeout);
			m_threads.push_back(*Thread::current);
			queue_lock.release();
			checker_lock.release();

			Thread::yield();

			checker_lock.acquire();
			queue_lock.acquire();
		}
	}

  private:
	UniquePointer<Spinlock> m_lock;
	Vector<Reference<Thread>> m_threads;
	bool is_moved = false;
	void wake_up_one();
};
