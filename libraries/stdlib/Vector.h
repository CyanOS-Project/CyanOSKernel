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
	template <typename Type> class BaseIterator
	{

	  public:
		BaseIterator(const BaseIterator& other);
		~BaseIterator() = default;
		void operator=(const BaseIterator& other);
		operator BaseIterator<const RemoveConst<Type>>();
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
		T* operator->();
		T& operator*();
		friend Vector<T>;

	  private:
		T* m_storage;
		size_t m_current;
		BaseIterator(T* storage, size_t index);
	};
	static_assert(is_RandomAccessIterator<BaseIterator<T>>, "Not valid Random Access Iterator.");

	void realloc(size_t new_size);
	void realloc_with_free_spot(size_t new_size, size_t free_spot_index);
	void make_free_spot(size_t free_spot_index);
	void destruct_used_objects();

  public:
	NON_COPYABLE(Vector);
	NON_MOVABLE(Vector);
	using ConstIterator = BaseIterator<const T>;
	using Iterator = BaseIterator<T>;

	explicit Vector(size_t reserved_size = 1, size_t allocation_steps = 1);
	~Vector();

	Iterator begin() const;
	Iterator end() const;
	ConstIterator cbegin() const;
	ConstIterator cend() const;
	template <typename... U> T& emplace_back(U&&... u);
	template <typename... U> T& emplace_front(U&&... u);
	template <typename... U> T& emplace(Iterator node, U&&... u);
	template <typename U> T& insert(Iterator node, U&& new_node);
	template <typename U> T& push_back(U&& new_data);
	template <typename U> T& push_front(U&& new_data);
	void pop_back();
	void pop_front();
	void erase(Iterator);
	void reserve(size_t size);
	template <class Predicate> bool remove_if(Predicate predicate);
	template <class Predicate> bool contains(Predicate predicate);
	void clear();
	void splice(Vector<T>& destination_list, Iterator from, Iterator to);
	bool is_empty() const;
	size_t size() const;
	size_t capacity() const;
	T& head() const;
	T& tail() const;
	T& operator[](size_t index) const;
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

template <class T> Vector<T>::~Vector()
{
	destruct_used_objects();
	::operator delete(m_storage);
}

template <class T> void Vector<T>::realloc(size_t size)
{
	ASSERT(size);
	T* new_storage = nullptr;
	if (size > m_capacity) {
		new_storage = reinterpret_cast<T*>(operator new(size * sizeof(T)));
		for (size_t i = 0; i < m_count; i++) {
			new_storage[i] = move(m_storage[i]);
		}
	} else {
		// FIXME:shrink ?
		ASSERT_NOT_REACHABLE();
	}
	destruct_used_objects();
	operator delete(m_storage);
	m_capacity = size;
	m_storage = new_storage;
}

template <class T> void Vector<T>::realloc_with_free_spot(size_t size, size_t free_spot_index)
{
	ASSERT(size);
	ASSERT(free_spot_index <= m_count);
	T* new_storage = nullptr;
	if (size > m_capacity) {
		new_storage = reinterpret_cast<T*>(operator new(size * sizeof(T)));
		for (size_t i = 0; i < free_spot_index; i++) {
			new_storage[i] = move(m_storage[i]);
		}

		for (size_t i = free_spot_index; i < m_count; i++) {
			new_storage[i + 1] = move(m_storage[i]);
		}
	} else {
		// FIXME:shrink ?
		ASSERT_NOT_REACHABLE();
	}
	destruct_used_objects();
	operator delete(m_storage);
	m_capacity = size;
	m_storage = new_storage;
}

template <class T> void Vector<T>::make_free_spot(size_t free_spot_index)
{
	for (size_t i = m_count; i >= free_spot_index + 1; i--) {
		m_storage[i] = move(m_storage[i - 1]);
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

template <class T> typename Vector<T>::Iterator Vector<T>::begin() const
{
	return Iterator(m_storage, 0);
}

template <class T> typename Vector<T>::Iterator Vector<T>::end() const
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
	m_storage[pos.m_current] = forward<U>(new_data);
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
	erase(end() - 1);
}

template <class T> void Vector<T>::pop_front()
{
	erase(begin());
}

template <class T> void Vector<T>::erase(Iterator itr)
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

template <typename T>
template <typename P>
Vector<T>::template BaseIterator<P>::BaseIterator(T* storage, size_t index) : m_storage{storage}, m_current{index}
{
}

template <typename T>
template <typename P>
Vector<T>::template BaseIterator<P>::BaseIterator(const BaseIterator& other) :
    m_storage{other.m_storage},
    m_current{other.m_current}
{
}

template <typename T>
template <typename P>
Vector<T>::template BaseIterator<P>::operator BaseIterator<const RemoveConst<P>>()
{
	return BaseIterator<const RemoveConst<P>>{m_storage, m_current};
}

template <typename T>
template <typename P>
void Vector<T>::template BaseIterator<P>::operator=(const BaseIterator<P>& other)
{
	m_current = other.m_current;
	m_storage = other.m_storage;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P> Vector<T>::template BaseIterator<P>::operator++(int arg)
{
	UNUSED(arg);
	auto old{*this};
	m_current++;
	return old;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P>& Vector<T>::template BaseIterator<P>::operator++()
{
	m_current++;
	return *this;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P> Vector<T>::template BaseIterator<P>::operator--(int arg)
{
	UNUSED(arg);
	auto old{*this};
	m_current--;
	return old;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P>& Vector<T>::template BaseIterator<P>::operator--()
{
	m_current--;
	return *this;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P>& Vector<T>::template BaseIterator<P>::operator+=(int arg)
{
	m_current += arg;
	return *this;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P>& Vector<T>::template BaseIterator<P>::operator-=(int arg)
{
	return *this += -arg;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P> Vector<T>::template BaseIterator<P>::operator+(int arg) const
{
	auto new_itr = *this;
	return new_itr += arg;
}

template <typename T>
template <typename P>
typename Vector<T>::template BaseIterator<P> Vector<T>::template BaseIterator<P>::operator-(int arg) const
{
	auto new_itr = *this;
	return new_itr -= arg;
}

template <typename T>
template <typename P>
bool Vector<T>::template BaseIterator<P>::operator!=(const BaseIterator<P>& other) const
{
	return m_current != other.m_current;
}

template <typename T>
template <typename P>
bool Vector<T>::template BaseIterator<P>::operator==(const BaseIterator<P>& other) const
{
	return m_current == other.m_current && m_storage == other.m_storage;
}

template <typename T> template <typename P> T& Vector<T>::template BaseIterator<P>::operator*()
{
	return m_storage[m_current];
}

template <typename T> template <typename P> T* Vector<T>::template BaseIterator<P>::operator->()
{
	return &m_storage[m_current];
}