#pragma once

#include <rule5.h>
#include <type_traits.h>

template <typename T> class ScopedLock
{
  public:
	NON_COPYABLE(ScopedLock)

	explicit ScopedLock(T& lock) : m_lock{lock}, m_locked{true} { acquire(); }
	ScopedLock(ScopedLock&& other) : m_lock{other.m_lock}, m_locked{other.m_locked} { other.m_locked = false; }
	ScopedLock& operator=(ScopedLock&& other)
	{
		// FIXME: is there a race condition here?
		m_lock = other.m_lock;
		m_locked = other.m_locked;
		other.m_locked = false;
		return *this;
	}

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
