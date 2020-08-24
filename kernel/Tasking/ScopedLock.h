#pragma once

#include "Utils/Rule5.h"

template <typename T> class ScopedLock
{
  private:
	T& m_lock;

  public:
	NON_COPYABLE(ScopedLock)
	NON_MOVABLE(ScopedLock)

	explicit ScopedLock(T& lock) : m_lock{lock}
	{
		acquire();
	}

	~ScopedLock()
	{
		release();
	}

	void release()
	{
		m_lock.release();
	}

	void acquire()
	{
		m_lock.acquire();
	}
};
