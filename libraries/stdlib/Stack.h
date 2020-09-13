#pragma once

#include <Algorithms.h>
#include <Types.h>
#ifdef __UNIT_TESTS
	#include <assert.h>
	#define ASSERT(x) assert(x)
#else
	#include <Assert.h>
#endif
template <typename T> class Stack
{
  private:
	T* m_data;
	size_t m_head;
	size_t m_size;
	const size_t m_capacity;

  public:
	explicit Stack(const size_t capacity);
	void queue(const T&);
	void queue(T&&);
	T dequeue();
	T& back();
	bool is_empty();
	bool is_full();
	size_t capacity();
	size_t size();
	size_t available_size();
	~Stack();
};

template <typename T>
Stack<T>::Stack(const size_t capacity) :
    m_data{new T[capacity]}, //
    m_head{0},
    m_size{0},
    m_capacity{capacity}
{
}

template <typename T> void Stack<T>::queue(const T& data_to_queue)
{
	ASSERT(!is_full());
	m_data[m_head] = data_to_queue;
	m_head++;
	m_size++;
}

template <typename T> void Stack<T>::queue(T&& data_to_queue)
{
	ASSERT(!is_full());
	m_data[m_head] = move(data_to_queue);
	m_head++;
	m_size++;
}

template <typename T> T Stack<T>::dequeue()
{
	ASSERT(!is_empty());
	T ret_data = move(m_data[m_head - 1]);
	m_data[m_head - 1].~T();
	m_head--;
	m_size--;
	return ret_data;
}

template <typename T> T& Stack<T>::back()
{
	return m_data[m_head - 1];
}

template <typename T> Stack<T>::~Stack()
{
	for (size_t i = 0; i < m_head; i++) {
		m_data[i].~T();
	}
	operator delete(m_data);
}

template <typename T> bool Stack<T>::is_empty()
{
	return m_size == 0;
}

template <typename T> bool Stack<T>::is_full()
{
	return m_size == m_capacity;
}

template <typename T> size_t Stack<T>::capacity()
{
	return m_capacity;
}

template <typename T> size_t Stack<T>::size()
{
	return m_size;
}

template <typename T> size_t Stack<T>::available_size()
{
	return capacity() - size();
}