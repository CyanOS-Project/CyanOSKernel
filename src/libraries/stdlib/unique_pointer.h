#pragma once

#include "algorithms.h"
#include "assert.h"
#include "rule5.h"

template <typename T> class UniquePointer
{
  public:
	NON_COPYABLE(UniquePointer)

	template <class... Args> static UniquePointer<T> make_unique(Args&&... args)
	{
		return UniquePointer(new T{forward<Args>(args)...});
	}

	explicit UniquePointer(T* ptr) : m_storage{ptr} { ASSERT(m_storage); }
	UniquePointer(UniquePointer&& other) : m_storage{other.release()} {}
	template <typename U> UniquePointer(UniquePointer<U>&& other) : m_storage{static_cast<T*>(other.release())} {}

	UniquePointer& operator=(UniquePointer&& other)
	{
		ASSERT(this != &other);
		reset(other.release());
		return *this;
	}

	template <typename U> UniquePointer& operator=(UniquePointer<U>&& other)
	{
		ASSERT(this != &other);
		reset(static_cast<T*>(other.release()));
		return *this;
	}

	~UniquePointer() { destroy(); }

	T& operator*() { return *m_storage; }
	T* operator->() { return m_storage; }
	T* ptr() { return m_storage; }
	const T& operator*() const { return *m_storage; }
	const T* operator->() const { return m_storage; }
	const T* ptr() const { return m_storage; }

	bool operator==(const UniquePointer& other) const { return (m_storage == other.m_storage); }
	bool operator!=(const UniquePointer& other) const { return (m_storage != other.m_storage); }

	T* release()
	{
		T* tmp = m_storage;
		m_storage = nullptr;
		return tmp;
	}

	void reset(T* new_ptr)
	{
		destroy();
		m_storage = new_ptr;
	}

  private:
	T* m_storage;

	void destroy()
	{
		delete m_storage;
		m_storage = nullptr;
	}
};

template <typename T> class UniquePointer<T[]>
{
  public:
	NON_COPYABLE(UniquePointer)

	template <class... Args> static UniquePointer<T[]> make_unique(size_t size, Args&&... args)
	{
		return UniquePointer(new T[size]{forward<Args>(args)...});
	}

	explicit UniquePointer(T* ptr) : m_storage{ptr} { ASSERT(m_storage); }
	UniquePointer(UniquePointer&& other) : m_storage{other.release()} {}

	UniquePointer& operator=(UniquePointer&& other)
	{
		ASSERT(this != &other);
		reset(other.release());
		return *this;
	}

	template <typename U> UniquePointer& operator=(UniquePointer<U>&& other)
	{
		ASSERT(this != &other);
		reset(static_cast<T*>(other.release()));
		return *this;
	}

	~UniquePointer() { destroy(); }

	T& operator[](size_t index) { return m_storage[index]; }
	T& operator*() { return *m_storage; }
	T* operator->() { return m_storage; }
	T* ptr() { return m_storage; }
	const T& operator[](size_t index) const { return m_storage[index]; }
	const T& operator*() const { return *m_storage; }
	const T* operator->() const { return m_storage; }
	const T* ptr() const { return m_storage; }

	bool operator==(const UniquePointer& other) const { return (m_storage == other.m_storage); }
	bool operator!=(const UniquePointer& other) const { return (m_storage != other.m_storage); }

	T* release()
	{
		T* tmp = m_storage;
		m_storage = nullptr;
		return tmp;
	}

	void reset(T* new_ptr)
	{
		destroy();
		m_storage = new_ptr;
	}

  private:
	T* m_storage;

	void destroy()
	{
		delete[] m_storage;
		m_storage = nullptr;
	}
};