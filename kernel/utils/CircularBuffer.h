#pragma once

#include "utils/stl.h"
#include "utils/types.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#define ASSERT(x) assert(x)
#else
	#include "assert.h"
#endif
template <typename T> class CircularBuffer
{
  private:
	T* m_data;
	size_t m_head;
	size_t m_tail;
	size_t m_size;
	const size_t m_max_size;

  public:
	explicit CircularBuffer(const size_t size);
	void queue(const T&);
	void queue(T&&);
	T dequeue();
	bool is_empty();
	bool is_full();
	size_t max_size();
	size_t size();
	size_t available_size();
	~CircularBuffer();
};

template <typename T>
CircularBuffer<T>::CircularBuffer(const size_t size) :
    m_data{new T[size]}, //
    m_head{0},
    m_tail{0},
    m_size{0},
    m_max_size{size}
{
}

template <typename T> void CircularBuffer<T>::queue(const T& data_to_queue)
{
	ASSERT(!is_full());
	m_data[m_tail] = data_to_queue;
	m_tail = (m_tail + 1) % m_max_size;
	m_size++;
}

template <typename T> void CircularBuffer<T>::queue(T&& data_to_queue)
{
	ASSERT(!is_full());
	m_data[m_tail] = move(data_to_queue);
	m_tail = (m_tail + 1) % m_max_size;
	m_size++;
}

template <typename T> T CircularBuffer<T>::dequeue()
{
	ASSERT(!is_empty());
	T ret_data = move(m_data[m_head]);
	m_data[m_head].~T();
	m_head = (m_head + 1) % m_max_size;
	m_size--;
	return ret_data;
}

template <typename T> CircularBuffer<T>::~CircularBuffer()
{
	delete m_data;
}

template <typename T> bool CircularBuffer<T>::is_empty()
{
	return m_size == 0;
}

template <typename T> bool CircularBuffer<T>::is_full()
{
	return m_size == m_max_size;
}

template <typename T> size_t CircularBuffer<T>::max_size()
{
	return m_max_size;
}

template <typename T> size_t CircularBuffer<T>::size()
{
	return m_size;
}

template <typename T> size_t CircularBuffer<T>::available_size()
{
	return max_size() - size();
}