#pragma once

#include "Algorithms.h"
#include "New.h"
#include "Rule5.h"
#include "Types.h"

#ifdef __UNIT_TESTS
	#include <assert.h>
	#define ASSERT(x)               assert(x)
	#define ASSERT_NOT_REACHABLE(x) assert(false)
#else
	#include "Assert.h"
#endif

template <class T> class Vector
{
  private:
	T* m_storage;
	size_t m_count;
	size_t m_capacity;
	size_t m_allocation_steps;
	class Iterator
	{
	  private:
		T* m_storage;
		size_t m_current;

	  public:
		Iterator(T* storage, size_t index);
		explicit Iterator(T* storage);
		Iterator(const Iterator& other);
		void operator=(const Vector<T>::Iterator& other);
		~Iterator() = default;

		Iterator operator++(int);
		Iterator& operator++();
		Iterator operator+(int);
		Iterator operator-(int);
		bool operator!=(const Vector<T>::Iterator& other) const;
		bool operator==(const Vector<T>::Iterator& other) const;
		T* operator->();
		T& operator*();

		friend Vector<T>;
	};

	void realloc(size_t new_size); // FIXME: no need for two functions.
	void realloc(size_t new_size, size_t skip);
	void destruct_used_objects();

  public:
	NON_COPYABLE(Vector);
	NON_MOVABLE(Vector);
	explicit Vector(size_t reserved_size = 1, size_t allocation_steps = 1);
	~Vector();

	Iterator begin();
	Iterator end();
	void reserve(size_t size);
	template <typename... U> T& emplace_back(U&&... u);
	template <typename... U> T& emplace_front(U&&... u);
	T& push_back(const T& new_data);
	T& push_front(const T& new_data);
	T& push_back(T&& new_data);
	T& push_front(T&& new_data);
	void pop_back();
	void pop_front();
	void insert(const Iterator& node, const T& new_node);
	void erase(const Iterator&);
	template <class Predicate> bool remove_if(Predicate predicate);
	template <class Predicate> bool contains(Predicate predicate);
	void clear();
	void splice(Vector<T>& list, const Iterator& itr);
	bool is_empty() const;
	size_t size() const;
	size_t capacity() const;
	T& head() const;
	T& tail() const;
	T& operator[](size_t index) const;
};

template <class T> Vector<T>::Iterator::Iterator(T* storage, size_t index) : m_storage{storage}, m_current{index} {}

template <class T>
Vector<T>::Iterator::Iterator(const Iterator& other) : m_storage{other.m_storage}, m_current{other.m_current}
{
}

template <class T> Vector<T>::Iterator::Iterator(T* storage) : m_storage{storage}, m_current{0} {}

template <class T> typename Vector<T>::Iterator Vector<T>::Iterator::operator++(int arg)
{
	UNUSED(arg);
	Iterator old{*this};
	m_current++;
	return old;
}

template <class T> typename Vector<T>::Iterator Vector<T>::Iterator::operator+(int arg)
{
	return Iterator{m_storage, m_current + arg};
}

template <class T> typename Vector<T>::Iterator Vector<T>::Iterator::operator-(int arg)
{
	return Iterator{m_storage, m_current - arg};
}

template <class T> typename Vector<T>::Iterator& Vector<T>::Iterator::operator++()
{
	m_current++;
	return *this;
}

template <class T> bool Vector<T>::Iterator::operator!=(const Vector<T>::Iterator& other) const
{
	return m_current != other.m_current;
}

template <class T> bool Vector<T>::Iterator::operator==(const Vector<T>::Iterator& other) const
{
	return m_current == other.m_current && m_storage == other.m_storage;
}

template <class T> void Vector<T>::Iterator::operator=(const Vector<T>::Iterator& other)
{
	m_current = other->m_current;
}

template <class T> T& Vector<T>::Iterator::operator*()
{
	return m_storage[m_current];
}

template <class T> T* Vector<T>::Iterator::operator->()
{
	return &m_storage[m_current];
}

template <class T>
Vector<T>::Vector(size_t reserved_size, size_t allocation_steps) :
    m_storage{nullptr},
    m_count{0},
    m_capacity{0},
    m_allocation_steps{allocation_steps}
{
	realloc(reserved_size);
}

template <class T> Vector<T>::~Vector()
{
	destruct_used_objects();
	::operator delete(m_storage);
}

template <class T> void Vector<T>::realloc(size_t size)
{
	if (!m_storage) {
		m_storage = reinterpret_cast<T*>(operator new(size * sizeof(T)));
	} else if (size > m_capacity) {
		T* new_storage = reinterpret_cast<T*>(operator new(size * sizeof(T)));
		for (size_t i = 0; i < m_count; i++) {
			new_storage[i] = move(m_storage[i]);
		}

		destruct_used_objects();
		operator delete(m_storage);

		m_storage = new_storage;
	} else if (size < m_capacity) {
		// FIXME:shrink ?
		ASSERT_NOT_REACHABLE();
	}
	m_capacity = size;
}

// realloc and moves all old storage to the new one but make space for one element at index skip_index.
template <class T> void Vector<T>::realloc(size_t size, size_t skip_index)
{
	ASSERT(size > m_capacity);
	ASSERT(skip_index < m_capacity);

	T* new_storage = reinterpret_cast<T*>(operator new(size * sizeof(T)));
	for (size_t i = 0; i < skip_index; i++) {
		new_storage[i] = move(m_storage[i]);
	}

	for (size_t i = skip_index; i < m_count; i++) {
		new_storage[i + 1] = move(m_storage[i]);
	}

	destruct_used_objects();
	operator delete(m_storage);

	m_storage = new_storage;
	m_capacity = size;
}

template <class T> void Vector<T>::destruct_used_objects()
{
	for (size_t i = 0; i < m_count; i++) {
		m_storage[i].~T();
	}
}

template <class T> void Vector<T>::reserve(size_t size)
{
	ASSERT(size > m_capacity);
	realloc(size);
}

template <class T> void Vector<T>::clear()
{
	destruct_used_objects();
	// FIXME: shrink ?
	m_count = 0;
}

template <class T> void Vector<T>::splice(Vector<T>& list, const Iterator& itr)
{
	list.push_back(m_storage[itr.m_current]);
	erase(itr);
}

template <class T> typename Vector<T>::Iterator Vector<T>::begin()
{
	return Iterator(m_storage);
}

template <class T> typename Vector<T>::Iterator Vector<T>::end()
{
	return Iterator(m_storage, m_count);
}

template <class T> template <typename... U> T& Vector<T>::emplace_back(U&&... u)
{
	if (m_capacity == m_count) {
		realloc(m_capacity + m_allocation_steps);
	}
	new (&m_storage[m_count]) T{forward<U>(u)...};
	m_count++;
	return m_storage[m_count - 1];
}

template <class T> template <typename... U> T& Vector<T>::emplace_front(U&&... u)
{
	if (m_capacity == m_count) {
		realloc(m_capacity + m_allocation_steps, 0);
	}
	new (&m_storage[0]) T{forward<U>(u)...};
	m_count++;
	return m_storage[m_count - 1];
}

template <class T> T& Vector<T>::push_back(const T& new_data)
{
	if (m_capacity == m_count) {
		realloc(m_capacity + m_allocation_steps);
	}
	m_storage[m_count] = new_data;
	m_count++;
	return m_storage[m_count - 1];
}

template <class T> T& Vector<T>::push_front(const T& new_data)
{
	if (m_capacity == m_count) {
		realloc(m_capacity + m_allocation_steps, 0);
	}
	m_storage[0] = new_data;
	m_count++;
	return m_storage[m_count - 1];
}

template <class T> T& Vector<T>::push_back(T&& new_data)
{
	if (m_capacity == m_count) {
		realloc(m_capacity + m_allocation_steps);
	}
	m_storage[m_count] = move(new_data);
	m_count++;
	return m_storage[m_count - 1];
}

template <class T> T& Vector<T>::push_front(T&& new_data)
{
	if (m_capacity == m_count) {
		realloc(m_capacity + m_allocation_steps, 0);
	}
	m_storage[0] = move(new_data);
	m_count++;
	return m_storage[m_count - 1];
}

template <class T> template <class Predicate> bool Vector<T>::remove_if(Predicate predicate)
{
	bool is_removed = false;
	auto&& i = begin();
	while (i != end()) {
		auto iterator_copy = i++;
		if (predicate(*iterator_copy)) {
			erase(iterator_copy);
			is_removed = true;
		}
	}
	return is_removed;
}

template <class T> template <class Predicate> bool Vector<T>::contains(Predicate predicate)
{
	for (auto&& i : *this) {
		if (predicate(i)) {
			return true;
		}
	}
	return false;
}

template <class T> void Vector<T>::pop_back()
{
	erase(m_count - 1);
}

template <class T> void Vector<T>::pop_front()
{
	erase(0);
}

template <class T> void Vector<T>::insert(const Iterator& pos, const T& new_data)
{
	ASSERT(m_storage);
	if (m_count == m_capacity) {
		realloc(m_capacity + m_allocation_steps, pos.m_current);
	}
	m_storage[pos.m_current] = new_data;
	m_count++;
}

template <class T> void Vector<T>::erase(const Iterator& itr)
{
	ASSERT(m_storage);
	for (size_t i = itr.m_current; i < m_count - 1; i++) {
		m_storage[i] = move(m_storage[i + 1]);
	}
	m_storage[m_count - 1].~T();
	m_count--;
}

template <class T> T& Vector<T>::head() const
{
	ASSERT(m_storage);
	return m_storage[0];
}

template <class T> T& Vector<T>::tail() const
{
	ASSERT(m_storage);
	return m_storage[m_count - 1];
}

template <class T> T& Vector<T>::operator[](size_t index) const
{
	ASSERT(index < m_count);
	ASSERT(m_storage);
	return m_storage[index];
}

template <class T> bool Vector<T>::is_empty() const
{
	return !size();
}

template <class T> size_t Vector<T>::size() const
{
	return m_count;
}

template <class T> size_t Vector<T>::capacity() const
{
	return m_capacity;
}