#pragma once

#include "Utils/Rule5.h"

template <typename T> class ScopedLock
{
  private:
	T& m_lock;
	bool m_locked = false;

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
};
