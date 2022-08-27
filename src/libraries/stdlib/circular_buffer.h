#pragma once

#include "algorithms.h"
#include "new.h"
#include "types.h"
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
	size_t m_capacity;

	void destroy_used_objects();

  public:
	explicit CircularBuffer(const size_t size);
	CircularBuffer(const CircularBuffer&);
	CircularBuffer(CircularBuffer&&);
	CircularBuffer& operator=(const CircularBuffer&);
	CircularBuffer& operator=(CircularBuffer&&);
	~CircularBuffer();

	template <typename U> void queue(U&&);
	T dequeue();
	T& front();
	T& back();
	const T& front() const;
	const T& back() const;
	bool is_empty() const;
	bool is_full() const;
	size_t capacity() const;
	size_t size() const;
	size_t available_size() const;
};

template <typename T>
CircularBuffer<T>::CircularBuffer(size_t size) :
    m_data{reinterpret_cast<T*>(operator new(sizeof(T) * size))},
    m_head{0},
    m_tail{0},
    m_size{0},
    m_capacity{size}
{
}

template <typename T>
CircularBuffer<T>::CircularBuffer(const CircularBuffer<T>& other) :
    m_data{reinterpret_cast<T*>(operator new(sizeof(T) * other.m_capacity))},
    m_head{other.m_head},
    m_tail{other.m_tail},
    m_size{other.m_size},
    m_capacity{other.m_capacity}
{
	size_t index = m_head;
	for (size_t i = 0; i < m_size; i++) {
		new (&m_data[index]) T{other.m_data[index]};
		index = (index + 1) % m_capacity;
	}
}

template <typename T>
CircularBuffer<T>::CircularBuffer(CircularBuffer<T>&& other) :
    m_data{other.m_data},
    m_head{other.m_head},
    m_tail{other.m_tail},
    m_size{other.m_size},
    m_capacity{other.m_capacity}
{
	other.m_data = nullptr;
	other.m_size = other.m_head = other.m_tail = 0;
}

template <typename T> CircularBuffer<T>& CircularBuffer<T>::operator=(const CircularBuffer<T>& other)
{
	if (&other != this) {
		if (m_capacity != other.m_capacity) {
			destroy_used_objects();
			operator delete(m_data);
			m_data = reinterpret_cast<T*>(operator new(sizeof(T) * m_capacity));
		} else {
			destroy_used_objects();
		}

		m_head = other.m_head;
		m_tail = other.m_tail;
		m_size = other.m_size;
		m_capacity = other.m_capacity;

		size_t index = m_head;
		for (size_t i = 0; i < m_size; i++) {
			new (&m_data[index]) T{other.m_data[index]};
			index = (index + 1) % m_capacity;
		}
	}
	return *this;
}

template <typename T> CircularBuffer<T>& CircularBuffer<T>::operator=(CircularBuffer<T>&& other)
{
	if (&other != this) {
		destroy_used_objects();
		operator delete(m_data);

		m_data = other.m_data;
		m_head = other.m_head;
		m_tail = other.m_tail;
		m_size = other.m_size;
		m_capacity = other.m_capacity;

		other.m_data = nullptr;
		other.m_size = other.m_head = other.m_tail = 0;
	}
	return *this;
}

template <typename T> CircularBuffer<T>::~CircularBuffer()
{
	destroy_used_objects();
	operator delete(m_data);
}

template <typename T> template <typename U> void CircularBuffer<T>::queue(U&& data_to_queue)
{
	ASSERT(!is_full());
	new (&m_data[m_tail]) T{forward<U>(data_to_queue)};
	m_tail = (m_tail + 1) % m_capacity;
	m_size++;
}

template <typename T> T CircularBuffer<T>::dequeue()
{
	ASSERT(!is_empty());
	T ret_data = move(m_data[m_head]);
	m_data[m_head].~T();
	m_head = (m_head + 1) % m_capacity;
	m_size--;
	return ret_data;
}

template <typename T> T& CircularBuffer<T>::front()
{
	ASSERT(!is_empty());
	return m_data[m_head];
}

template <typename T> T& CircularBuffer<T>::back()
{
	ASSERT(!is_empty());
	if (m_tail == 0) {
		return m_data[m_capacity - 1];
	} else {
		return m_data[m_tail - 1];
	}
}

template <typename T> const T& CircularBuffer<T>::front() const
{
	ASSERT(!is_empty());
	return m_data[m_head];
}

template <typename T> const T& CircularBuffer<T>::back() const
{
	ASSERT(!is_empty());
	if (m_tail == 0) {
		return m_data[m_capacity - 1];
	} else {
		return m_data[m_tail - 1];
	}
}

template <typename T> bool CircularBuffer<T>::is_empty() const
{
	return m_size == 0;
}

template <typename T> bool CircularBuffer<T>::is_full() const
{
	return m_size == m_capacity;
}

template <typename T> size_t CircularBuffer<T>::capacity() const
{
	return m_capacity;
}

template <typename T> size_t CircularBuffer<T>::size() const
{
	return m_size;
}

template <typename T> size_t CircularBuffer<T>::available_size() const
{
	return capacity() - size();
}

template <typename T> void CircularBuffer<T>::destroy_used_objects()
{
	size_t index = m_head;
	for (size_t i = 0; i < m_size; i++) {
		m_data[index].~T();
		index = (index + 1) % m_capacity;
	}
}