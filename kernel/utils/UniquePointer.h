#pragma once

#include "assert.h"
#include "stl.h"

template <typename T> class UniquePointer
{
  public:
	template <class... Args> static UniquePointer<T> make_unique(Args&&... args)
	{
		return UniquePointer(new T{forward<Args>(args)...});
	}

	explicit UniquePointer(T* ptr) : m_storage{ptr}
	{
		assert(m_storage);
	}

	UniquePointer(UniquePointer&& other) : m_storage{other.m_storage}
	{
		other.m_storage = nullptr;
	}

	UniquePointer& operator=(UniquePointer&& other)
	{
		delete m_storage;
		m_storage = other.m_storage;
		other.m_storage = nullptr;
		return *this;
	}

	UniquePointer(const UniquePointer& other) = delete;
	UniquePointer& operator=(const UniquePointer& other) = delete;
	~UniquePointer()
	{
		delete m_storage;
		m_storage = nullptr;
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
		delete m_storage;
		m_storage = nullptr;
		return m_storage;
	}

	T* get()
	{
		return m_storage;
	}

  private:
	T* m_storage;
};