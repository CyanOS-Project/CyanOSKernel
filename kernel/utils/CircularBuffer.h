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
	size_t m_count;
	const size_t m_size;

  public:
	explicit CircularBuffer(const size_t size);
	void queue(const T&);
	void queue(T&&);
	T dequeue();
	inline bool is_empty()
	{
		return m_count == 0;
	}
	bool is_full()
	{
		return m_count == m_size;
	}
	size_t size()
	{
		return m_size;
	};
	size_t count()
	{
		return m_count;
	};
	~CircularBuffer();
};

template <typename T>
CircularBuffer<T>::CircularBuffer(const size_t size) :
    m_data{new T[size]}, //
    m_head{0},
    m_tail{0},
    m_count{0},
    m_size{size}
{
}

template <typename T> void CircularBuffer<T>::queue(const T& data_to_queue)
{
	ASSERT(!is_full());
	m_data[m_tail] = data_to_queue;
	m_tail = (m_tail + 1) % m_size;
	m_count++;
}

template <typename T> void CircularBuffer<T>::queue(T&& data_to_queue)
{
	ASSERT(!is_full());
	m_data[m_tail] = move(data_to_queue);
	m_tail = (m_tail + 1) % m_size;
	m_count++;
}

template <typename T> T CircularBuffer<T>::dequeue()
{
	ASSERT(!is_empty());
	T ret_data = move(m_data[m_head]);
	m_data[m_head].~T();
	m_head = (m_head + 1) % m_size;
	m_count--;
	return ret_data;
}

template <typename T> CircularBuffer<T>::~CircularBuffer()
{
	delete m_data;
}