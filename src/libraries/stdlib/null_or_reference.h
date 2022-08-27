#pragma once
#include "assert.h"

template <typename T> class NullOrReference
{
  public:
	DEFAULT_COPY(NullOrReference)
	DEFAULT_MOVE(NullOrReference)
	DEFAULT_DESTRUCTOR(NullOrReference)

	NullOrReference() { m_storage = nullptr; }
	NullOrReference(T* storage) : m_storage{storage} { ASSERT(storage); }
	NullOrReference(T& storage) : m_storage{&storage} {}
	T& value()
	{
		ASSERT(is_valid());
		return *m_storage;
	}
	T* ptr()
	{
		ASSERT(is_valid());
		return m_storage;
	}
	T& operator*()
	{
		ASSERT(is_valid());
		return *m_storage;
	}
	T* operator->()
	{
		ASSERT(is_valid());
		return m_storage;
	}

	const T& value() const
	{
		ASSERT(is_valid());
		return *m_storage;
	}
	const T* ptr() const
	{
		ASSERT(is_valid());
		return m_storage;
	}
	const T& operator*() const
	{
		ASSERT(is_valid());
		return *m_storage;
	}
	const T* operator->() const
	{
		ASSERT(is_valid());
		return m_storage;
	}
	bool is_valid() const { return m_storage != nullptr; }

  private:
	T* m_storage;
};