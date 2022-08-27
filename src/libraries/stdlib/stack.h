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
template <typename T> class Stack
{
  private:
	T* m_data;
	size_t m_size;
	size_t m_capacity;

	void destroy_used_objects();

  public:
	explicit Stack(const size_t capacity);
	Stack(const Stack&);
	Stack(Stack&&);
	Stack& operator=(const Stack&);
	Stack& operator=(Stack&&);
	~Stack();

	template <typename U> void push(U&&);
	T pop();
	T& back();
	const T& back() const;
	bool is_empty();
	bool is_full();
	size_t capacity();
	size_t size();
	size_t available_size();
};

template <typename T>
Stack<T>::Stack(const size_t capacity) :
    m_data{reinterpret_cast<T*>(operator new(sizeof(T) * capacity))},
    m_size{0},
    m_capacity{capacity}
{
}

template <typename T>
Stack<T>::Stack(const Stack& other) :
    m_data{reinterpret_cast<T*>(operator new(sizeof(T) * other.m_capacity))},
    m_size{other.m_size},
    m_capacity{other.m_capacity}
{
	for (size_t i = 0; i < m_size; i++) {
		new (&m_data[i]) T{other.m_data[i]};
	}
}

template <typename T>
Stack<T>::Stack(Stack&& other) : m_data{other.m_data}, m_size{other.m_size}, m_capacity{other.m_capacity}
{
	other.m_data = nullptr;
	other.m_size = 0;
}

template <typename T> Stack<T>& Stack<T>::operator=(const Stack& other)
{
	if (&other != this) {
		if (m_capacity != other.m_capacity) {
			destroy_used_objects();
			operator delete(m_data);
			m_data = reinterpret_cast<T*>(operator new(sizeof(T) * m_capacity));
		} else {
			destroy_used_objects();
		}

		m_size = other.m_size;
		m_capacity = other.m_capacity;
		for (size_t i = 0; i < m_size; i++) {
			new (&m_data[i]) T{other.m_data[i]};
		}
	}
	return *this;
}

template <typename T> Stack<T>& Stack<T>::operator=(Stack&& other)
{
	if (&other != this) {
		destroy_used_objects();
		operator delete(m_data);

		m_size = other.m_size;
		m_data = other.m_data;
		m_capacity = other.m_capacity;

		other.m_data = nullptr;
		other.m_size = 0;
	}
	return *this;
}

template <typename T> template <typename U> void Stack<T>::push(U&& data_to_queue)
{
	ASSERT(!is_full());
	new (&m_data[m_size++]) T{forward<U>(data_to_queue)};
}

template <typename T> T Stack<T>::pop()
{
	ASSERT(!is_empty());
	T ret_data = move(m_data[m_size - 1]);
	m_data[m_size - 1].~T();
	m_size--;
	return ret_data;
}

template <typename T> T& Stack<T>::back()
{
	return m_data[m_size - 1];
}

template <typename T> const T& Stack<T>::back() const
{
	return m_data[m_size - 1];
}

template <typename T> void Stack<T>::destroy_used_objects()
{
	for (size_t i = 0; i < m_size; i++) {
		m_data[i].~T();
	}
}

template <typename T> Stack<T>::~Stack()
{
	destroy_used_objects();
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