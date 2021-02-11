#pragma once

#include "Algorithms.h"
#include "Concepts.h"
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
	template <typename U> class BaseIterator
	{

	  public:
		BaseIterator(const BaseIterator& other);
		~BaseIterator() = default;
		void operator=(const BaseIterator& other);
		operator BaseIterator<const RemoveConst<U>>();
		BaseIterator operator++(int);
		BaseIterator& operator++();
		BaseIterator operator--(int);
		BaseIterator& operator--();
		BaseIterator operator+(int) const;
		BaseIterator operator-(int) const;
		BaseIterator& operator+=(int);
		BaseIterator& operator-=(int);
		bool operator!=(const BaseIterator& other) const;
		bool operator==(const BaseIterator& other) const;
		U* operator->();
		U& operator*();
		friend Vector<T>;

	  private:
		T* m_storage;
		size_t m_current;
		BaseIterator(T* storage, size_t index);
	};

	void realloc(size_t new_size);
	void copy_storage(const T*, size_t);
	void realloc_with_free_spot(size_t new_size, size_t free_spot_index);
	void make_free_spot(size_t free_spot_index);
	void destruct_used_objects();

  public:
	using ConstIterator = BaseIterator<const T>;
	using Iterator = BaseIterator<T>;
	static_assert(validate_RandomAccessIterator<Iterator, ConstIterator>, "Not valid Random Access Iterator.");

	explicit Vector(size_t reserved_size = 1, size_t allocation_steps = 1);
	Vector(const Vector&);
	Vector(Vector&&);
	Vector& operator=(const Vector&);
	Vector& operator=(Vector&&);
	~Vector();

	Iterator begin();
	Iterator end();
	template <typename... U> T& emplace_back(U&&... u);
	template <typename... U> T& emplace_front(U&&... u);
	template <typename... U> T& emplace(Iterator node, U&&... u);
	template <typename U> T& insert(Iterator node, U&& new_node);
	template <typename U> T& push_back(U&& new_data);
	template <typename U> T& push_front(U&& new_data);
	void reserve(size_t size);
	void pop_back();
	void pop_front();
	void erase(Iterator);
	void clear();
	template <class Predicate> bool remove_if(Predicate predicate);
	Iterator find(const T& element);
	template <class Predicate> Iterator find_if(Predicate predicate);
	void splice(Vector<T>& destination_list, Iterator from, Iterator to);
	T& head();
	T& tail();
	T& operator[](size_t index);

	ConstIterator cbegin() const;
	ConstIterator cend() const;
	ConstIterator find(const T& element) const;
	template <class Predicate> ConstIterator find_if(Predicate predicate) const;
	template <class Predicate> bool contains(Predicate predicate) const;
	bool is_empty() const;
	size_t size() const;
	size_t capacity() const;
	const T& head() const;
	const T& tail() const;
	const T& operator[](size_t index) const;
};

template <class T>
Vector<T>::Vector(size_t reserved_size, size_t allocation_steps) :
    m_storage{nullptr},
    m_count{0},
    m_capacity{0},
    m_allocation_steps{allocation_steps}
{
	ASSERT(reserved_size);
	ASSERT(allocation_steps);
	realloc(reserved_size);
}

template <class T>
Vector<T>::Vector(const Vector& other) :
    m_storage{other.m_storage},
    m_count{other.m_count},
    m_capacity{other.m_capacity},
    m_allocation_steps{other.m_allocation_steps}
{
	m_storage = reinterpret_cast<T*>(operator new(m_count * sizeof(T)));
	copy_storage(other.m_storage, m_count);
}

template <class T>
Vector<T>::Vector(Vector&& other) :
    m_storage{other.m_storage},
    m_count{other.m_count},
    m_capacity{other.m_capacity},
    m_allocation_steps{other.m_allocation_steps}
{
	other.m_storage = nullptr;
	other.m_count = 0;
}

template <class T> Vector<T>& Vector<T>::operator=(const Vector& other)
{
	// FIXME: Should assignment of a container, assign the new values to the conflicted values or destroy them all
	// then construct new value?
	if (&other != this) {
		if (m_capacity != other.m_capacity) {
			destruct_used_objects();
			operator delete(m_storage);
			m_storage = reinterpret_cast<T*>(operator new(other.m_count * sizeof(T)));
		} else {
			destruct_used_objects();
		}

		m_count = other.m_count;
		m_capacity = other.m_capacity;
		m_allocation_steps = other.m_allocation_steps;
		copy_storage(other.m_storage, m_count);
	}
	return *this;
}

template <class T> Vector<T>& Vector<T>::operator=(Vector&& other)
{
	if (&other != this) {
		destruct_used_objects();
		operator delete(m_storage);
		m_count = other.m_count;
		m_capacity = other.m_capacity;
		m_allocation_steps = other.m_allocation_steps;
		m_storage = other.m_storage;
		other.m_storage = nullptr;
		other.m_count = 0;
	}
	return *this;
}

template <class T> Vector<T>::~Vector()
{
	destruct_used_objects();
	operator delete(m_storage);
}

template <class T> void Vector<T>::copy_storage(const T* other_storage, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		new (&m_storage[i]) T{other_storage[i]};
	}
}

template <class T> void Vector<T>::realloc(size_t new_capacity)
{
	ASSERT(new_capacity);
	T* new_storage = nullptr;
	if (new_capacity > m_capacity) {
		new_storage = reinterpret_cast<T*>(operator new(new_capacity * sizeof(T)));
		for (size_t i = 0; i < m_count; i++) {
			new (&new_storage[i]) T{move(m_storage[i])};
		}
	} else {
		// FIXME:shrink ?
		ASSERT_NOT_REACHABLE();
	}
	destruct_used_objects();
	operator delete(m_storage);
	m_capacity = new_capacity;
	m_storage = new_storage;
}

template <class T> void Vector<T>::realloc_with_free_spot(size_t new_capacity, size_t free_spot_index)
{
	ASSERT(new_capacity >= m_count + 1);
	ASSERT(free_spot_index <= m_count);
	T* new_storage = nullptr;
	if (new_capacity > m_capacity) {
		new_storage = reinterpret_cast<T*>(operator new(new_capacity * sizeof(T)));
		for (size_t i = 0; i < free_spot_index; i++) {
			new (&new_storage[i]) T{move(m_storage[i])};
		}

		for (size_t i = free_spot_index; i < m_count; i++) {
			new (&new_storage[i + 1]) T{move(m_storage[i])};
		}
	} else {
		// FIXME:shrink ?
		ASSERT_NOT_REACHABLE();
	}
	destruct_used_objects();
	operator delete(m_storage);
	m_capacity = new_capacity;
	m_storage = new_storage;
}

template <class T> void Vector<T>::make_free_spot(size_t free_spot_index)
{
	for (size_t i = m_count; i >= free_spot_index + 1; i--) {
		m_storage[i] = move(m_storage[i - 1]);
		m_storage[i - 1].~T();
	}
}

template <class T> void Vector<T>::destruct_used_objects()
{
	for (size_t i = 0; i < m_count; i++) {
		m_storage[i].~T();
	}
}

template <class T> void Vector<T>::reserve(size_t size)
{
	if (m_capacity < size) {
		realloc(size);
	}
}

template <class T> void Vector<T>::clear()
{
	destruct_used_objects();
	// FIXME: shrink ?
	m_count = 0;
}

template <class T> void Vector<T>::splice(Vector<T>& destination_list, Iterator from, Iterator to)
{
	destination_list.insert(to, *from);
	erase(from);
}

template <class T> typename Vector<T>::ConstIterator Vector<T>::cbegin() const
{
	return ConstIterator(m_storage, 0);
}

template <class T> typename Vector<T>::ConstIterator Vector<T>::cend() const
{
	return ConstIterator(m_storage, m_count);
}

template <class T> typename Vector<T>::Iterator Vector<T>::begin()
{
	return Iterator(m_storage, 0);
}

template <class T> typename Vector<T>::Iterator Vector<T>::end()
{
	return Iterator(m_storage, m_count);
}

template <class T> template <typename... U> T& Vector<T>::emplace(Iterator pos, U&&... u)
{
	ASSERT(m_storage);
	if (m_count == m_capacity) {
		realloc_with_free_spot(m_capacity + m_allocation_steps, pos.m_current);
	} else {
		make_free_spot(pos.m_current);
	}
	new (&m_storage[pos.m_current]) T{forward<U>(u)...};
	m_count++;
	return m_storage[pos.m_current];
}

template <class T> template <typename U> T& Vector<T>::insert(Iterator pos, U&& new_data)
{
	ASSERT(m_storage);
	if (m_count == m_capacity) {
		realloc_with_free_spot(m_capacity + m_allocation_steps, pos.m_current);
	} else {
		make_free_spot(pos.m_current);
	}
	new (&m_storage[pos.m_current]) T{forward<U>(new_data)};
	m_count++;
	return m_storage[pos.m_current];
}

template <class T> template <typename... U> T& Vector<T>::emplace_back(U&&... u)
{
	return emplace(end(), forward<U>(u)...);
}

template <class T> template <typename... U> T& Vector<T>::emplace_front(U&&... u)
{
	return emplace(begin(), forward<U>(u)...);
}

template <class T> template <typename U> T& Vector<T>::push_back(U&& new_data)
{
	return insert(end(), forward<T>(new_data));
}

template <class T> template <typename U> T& Vector<T>::push_front(U&& new_data)
{
	return insert(begin(), forward<T>(new_data));
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

template <class T> typename Vector<T>::Iterator Vector<T>::find(const T& element)
{
	for (auto&& i = begin(); i != end(); i++) {
		if (element == *i) {
			return i;
		}
	}
	return end();
}

template <class T> template <class Predicate> typename Vector<T>::Iterator Vector<T>::find_if(Predicate predicate)
{
	for (auto&& i = begin(); i != end(); i++) {
		if (predicate(*i)) {
			return i;
		}
	}
	return end();
}

template <class T> typename Vector<T>::ConstIterator Vector<T>::find(const T& element) const
{
	for (auto&& i = cbegin(); i != cend(); i++) {
		if (element == *i) {
			return i;
		}
	}
	return end();
}

template <class T>
template <class Predicate>
typename Vector<T>::ConstIterator Vector<T>::find_if(Predicate predicate) const
{
	for (auto&& i = cbegin(); i != cend(); i++) {
		if (predicate(*i)) {
			return i;
		}
	}
	return end();
}

template <class T> template <class Predicate> bool Vector<T>::contains(Predicate predicate) const
{
	return find_if(predicate) != cend();
}

template <class T> void Vector<T>::pop_back()
{
	erase(--end());
}

template <class T> void Vector<T>::pop_front()
{
	erase(begin());
}

template <class T> void Vector<T>::erase(Iterator itr)
{
	ASSERT(m_storage);

	T tmp = move(m_storage[itr.m_current]); // not really used, it will be destructed at the end of the scope.
	UNUSED(tmp);
	for (size_t i = itr.m_current; i < m_count - 1; i++) {
		m_storage[i] = move(m_storage[i + 1]);
	}
	m_storage[m_count - 1].~T();
	m_count--;
}

template <class T> const T& Vector<T>::head() const
{
	ASSERT(m_storage);
	return m_storage[0];
}

template <class T> T& Vector<T>::head()
{
	ASSERT(m_storage);
	return m_storage[0];
}

template <class T> T& Vector<T>::tail()
{
	ASSERT(m_storage);
	return m_storage[m_count - 1];
}

template <class T> const T& Vector<T>::tail() const
{
	ASSERT(m_storage);
	return m_storage[m_count - 1];
}

template <class T> T& Vector<T>::operator[](size_t index)
{
	ASSERT(index < m_count);
	ASSERT(m_storage);
	return m_storage[index];
}

template <class T> const T& Vector<T>::operator[](size_t index) const
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

template <typename T>
template <typename U>
Vector<T>::BaseIterator<U>::BaseIterator(T* storage, size_t index) : m_storage{storage}, m_current{index}
{
}

template <typename T>
template <typename U>
Vector<T>::BaseIterator<U>::BaseIterator(const BaseIterator& other) :
    m_storage{other.m_storage},
    m_current{other.m_current}
{
}

template <typename T> template <typename U> Vector<T>::BaseIterator<U>::operator BaseIterator<const RemoveConst<U>>()
{
	return BaseIterator<const RemoveConst<U>>{m_storage, m_current};
}

template <typename T> template <typename U> void Vector<T>::BaseIterator<U>::operator=(const BaseIterator<U>& other)
{
	m_current = other.m_current;
	m_storage = other.m_storage;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U> Vector<T>::BaseIterator<U>::operator++(int arg)
{
	UNUSED(arg);
	auto old{*this};
	m_current++;
	return old;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U>& Vector<T>::BaseIterator<U>::operator++()
{
	m_current++;
	return *this;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U> Vector<T>::BaseIterator<U>::operator--(int arg)
{
	UNUSED(arg);
	auto old{*this};
	m_current--;
	return old;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U>& Vector<T>::BaseIterator<U>::operator--()
{
	m_current--;
	return *this;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U>& Vector<T>::BaseIterator<U>::operator+=(int arg)
{
	m_current += arg;
	return *this;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U>& Vector<T>::BaseIterator<U>::operator-=(int arg)
{
	return *this += -arg;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U> Vector<T>::BaseIterator<U>::operator+(int arg) const
{
	auto new_itr = *this;
	return new_itr += arg;
}

template <typename T>
template <typename U>
typename Vector<T>::BaseIterator<U> Vector<T>::BaseIterator<U>::operator-(int arg) const
{
	auto new_itr = *this;
	return new_itr -= arg;
}

template <typename T>
template <typename U>
bool Vector<T>::BaseIterator<U>::operator!=(const BaseIterator<U>& other) const
{
	return m_current != other.m_current;
}

template <typename T>
template <typename U>
bool Vector<T>::BaseIterator<U>::operator==(const BaseIterator<U>& other) const
{
	return m_current == other.m_current && m_storage == other.m_storage;
}

template <typename T> template <typename U> U& Vector<T>::BaseIterator<U>::operator*()
{
	return m_storage[m_current];
}

template <typename T> template <typename U> U* Vector<T>::BaseIterator<U>::operator->()
{
	return &m_storage[m_current];
}