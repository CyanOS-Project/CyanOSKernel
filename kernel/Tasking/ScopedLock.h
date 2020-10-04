#pragma once

#include <Rule5.h>

template <typename T> class ScopedLock
{
  public:
	NON_COPYABLE(ScopedLock)
	NON_MOVABLE(ScopedLock)

	explicit ScopedLock(T& lock) : m_lock{lock}, m_locked{true} { acquire(); }

	~ScopedLock()
	{
		if (m_locked)
			release();
	}

	void release()
	{
		m_lock.release();
		m_locked = false;
	}

	void acquire()
	{
		m_lock.acquire();
		m_locked = true;
	}

  private:
	T& m_lock;
	bool m_locked = false;
};
