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
	enum class State
	{
		Blocked,
		WokenUp,
		Timeout
	};

	NON_COPYABLE(WaitQueue)
	WaitQueue();
	WaitQueue(WaitQueue&&);
	WaitQueue& operator=(WaitQueue&&);
	~WaitQueue();

	void handle_thread_timeout(Thread&);
	void handle_thread_terminated(Thread&);
	void wake_up();
	void wake_up_all();
	template <typename T> State wait(ScopedLock<T>& checker_lock, size_t ms_timeout = 0)
	{
		ScopedLock queue_lock(*m_lock);

		Thread::current->block(*this, ms_timeout);
		auto blocked_thread = m_queue.emplace_back(*Thread::current);

		queue_lock.release();
		checker_lock.release();

		Thread::yield();

		checker_lock.acquire();
		queue_lock.acquire();

		State state = blocked_thread->state;
		m_queue.remove(blocked_thread);

		return state;
	}
	template <typename T, typename L> State wait_on_event(T checker, ScopedLock<L>& checker_lock, size_t ms_timeout = 0)
	{
		ScopedLock queue_lock(*m_lock);

		if (!checker()) {
			return State::WokenUp;
		}

		Vector<Queue>::Iterator blocked_thread;

		do {
			Thread::current->block(*this, ms_timeout);
			blocked_thread = m_queue.emplace_back(*Thread::current);
			queue_lock.release();
			checker_lock.release();

			Thread::yield();

			checker_lock.acquire();
			queue_lock.acquire();

		} while (checker() && (blocked_thread->state != State::Timeout));

		State state = blocked_thread->state;
		m_queue.remove(blocked_thread);
		return state;
	}

  private:
	struct Queue {
		Reference<Thread> thread;
		State state;
		Queue(Reference<Thread> t) : thread{t}, state{State::Blocked} {}
	};

	UniquePointer<Spinlock> m_lock;
	Vector<Queue> m_queue;
	bool is_moved = false;
};
