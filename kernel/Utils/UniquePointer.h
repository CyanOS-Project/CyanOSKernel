#pragma once

#include "Algorithms.h"
#include "Assert.h"
#include "Rule5.h"

template <typename T> class UniquePointer
{
  public:
	NON_COPYABLE(UniquePointer);

	template <class... Args> static UniquePointer<T> make_unique(Args&&... args)
	{
		return UniquePointer(new T{forward<Args>(args)...});
	}

	explicit UniquePointer(T* ptr) : m_storage{ptr}
	{
		ASSERT(m_storage);
	}

	UniquePointer(UniquePointer&& other) : m_storage{other.m_storage}
	{
		other.m_storage = nullptr;
	}

	UniquePointer& operator=(UniquePointer&& other)
	{
		ASSERT(this != &other);
		delete m_storage;
		m_storage = other.m_storage;
		other.m_storage = nullptr;
		return *this;
	}

	~UniquePointer()
	{
		destroy();
	}

	T& operator*()
	{
		return *m_storage;
	}

	T* operator->()
	{
		return m_storage;
	}

	bool operator==(const UniquePointer& other)
	{
		return (m_storage == other.m_storage);
	}

	bool operator!=(const UniquePointer& other)
	{
		return (m_storage != other.m_storage);
	}

	T* release()
	{
		m_storage = nullptr;
		return m_storage;
	}

	T* reset()
	{
		T* tmp = m_storage;
		destroy();
		return tmp;
	}

	T* ptr()
	{
		return m_storage;
	}

  private:
	T* m_storage;

	void destroy()
	{
		delete m_storage;
		m_storage = nullptr;
	}
};