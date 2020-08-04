#pragma once

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
	T m_data[];
	size_t m_head;
	size_t m_tail;
	size_t m_current_size;
	const size_t m_size;

  public:
	explicit CircularBuffer(size_t size);
	void queue(const T&);
	T& pop();
	inline bool is_empty()
	{
		return m_current_size == 0;
	}
	bool is_full()
	{
		return m_current_size == m_size;
	}
	size_t size()
	{
		return m_current_size;
	};
	~CircularBuffer();
};

template <typename T>
CircularBuffer<T>::CircularBuffer(size_t size) :
    m_data{new T[size]}, //
    m_head{0},
    m_tail{0},
    m_current_size{0},
    m_size{size}
{
}

template <typename T> void CircularBuffer<T>::queue(const T& data_to_queue)
{
	ASSERT(!is_full());
	m_data[m_tail] = data_to_queue;
	m_tail = (++m_tail) % m_size;
	m_current_size++;
}

template <typename T> T& CircularBuffer<T>::pop()
{
	ASSERT(!is_empty);
	T& ret_data = m_data[m_head];
	m_head = (++m_head) % m_head;
	m_current_size--;
}

template <typename T> CircularBuffer<T>::~CircularBuffer()
{
	delete[] m_data;
}