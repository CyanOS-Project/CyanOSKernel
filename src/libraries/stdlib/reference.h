#pragma once
#include "assert.h"

template <typename T> class Reference
{
  public:
	DEFAULT_COPY(Reference)
	DEFAULT_MOVE(Reference)
	DEFAULT_DESTRUCTOR(Reference)

	Reference(T* storage) : m_storage{storage} { ASSERT(storage); }
	Reference(T& storage) : m_storage{&storage} {}
	T& value() { return *m_storage; }
	T* ptr() { return m_storage; }
	T& operator*() { return *m_storage; }
	T* operator->() { return m_storage; }

	const T& value() const { return *m_storage; }
	const T* ptr() const { return m_storage; }
	const T& operator*() const { return *m_storage; }
	const T* operator->() const { return m_storage; }

  private:
	T* m_storage;
};