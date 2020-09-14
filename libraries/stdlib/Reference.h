#pragma once
#include <Assert.h>

template <typename T> class Reference
{
  public:
	Reference(T*);
	Reference(T&);
	Reference(Reference&&);
	Reference(const Reference&);
	Reference& operator=(Reference&&);
	Reference& operator=(const Reference&);
	~Reference();
	T& value();
	T* ptr();
	T& operator*();
	T* operator->();

  private:
	T* m_storage;
};

template <typename T> Reference<T>::Reference(T* storage) : m_storage{storage}
{
	ASSERT(storage);
}

template <typename T> Reference<T>::Reference(T& storage) : m_storage{&storage} {}

template <typename T> Reference<T>::Reference(const Reference<T>& other)
{
	ASSERT(other.m_storage);
	m_storage { other.m_storage }
}

template <typename T> Reference<T>::Reference(Reference<T>&& other)
{
	ASSERT(other.m_storage);
	m_storage { move(other.m_storage) }
}

template <typename T> Reference<T>& Reference<T>::operator=(const Reference<T>& other)
{
	ASSERT(other.m_storage);
	m_storage { other.m_storage }
}

template <typename T> Reference<T>& Reference<T>::operator=(Reference<T>&& other)
{
	ASSERT(other.m_storage);
	m_storage { move(other.m_storage) }
}

template <typename T> Reference<T>::~Reference() {}

template <typename T> T& Reference<T>::value()
{
	ASSERT(m_storage);
	return *m_storage;
}

template <typename T> T* Reference<T>::ptr()
{
	ASSERT(m_storage);
	return m_storage;
}

template <typename T> T& Reference<T>::operator*()
{
	ASSERT(m_storage);
	return *m_storage;
}

template <typename T> T* Reference<T>::operator->()
{
	ASSERT(m_storage);
	return m_storage;
}