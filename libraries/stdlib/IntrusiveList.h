#pragma once

#include "Algorithms.h"
#include "Concepts.h"
#include "Rule5.h"
#include "TypeTraits.h"
#include "Types.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#define ASSERT(x) assert(x)
#else
	#include "Assert.h"
#endif

template <typename T> class IntrusiveList;
// Inherit from this class to use it in IntrusiveList
template <typename T> class IntrusiveListNode
{
  public:
	IntrusiveListNode() = default;

  private:
	IntrusiveListNode(IntrusiveListNode<T>* t_next, IntrusiveListNode<T>* t_prev, IntrusiveList<T>* t_owner) :
	    next{t_next},
	    prev{t_prev},
	    owner{t_owner} {};
	IntrusiveListNode<T>* next = nullptr;
	IntrusiveListNode<T>* prev = nullptr;
	IntrusiveList<T>* owner = nullptr;
	friend class IntrusiveList<T>;
};

template <typename T> class IntrusiveList
{
  private:
	size_t m_count;
	IntrusiveListNode<T>* m_head;
	IntrusiveListNode<T>* m_tail;
	mutable IntrusiveListNode<T> m_ending_node;

	void remove_node(IntrusiveListNode<T>& node);
	void append_node(IntrusiveListNode<T>& node);
	bool is_valid_node(const IntrusiveListNode<T>& node) const;

	template <typename U> class BaseIterator
	{
	  public:
		BaseIterator(const BaseIterator& other);
		BaseIterator& operator=(const BaseIterator& other);
		~BaseIterator() = default;
		operator BaseIterator<const RemoveConst<U>>();
		bool operator==(const BaseIterator& other) const;
		bool operator!=(const BaseIterator& other) const;
		BaseIterator& operator++();
		BaseIterator operator++(int);
		BaseIterator& operator--();
		BaseIterator operator--(int);
		U& operator*();
		U* operator->();

	  private:
		IntrusiveListNode<T>* m_node = nullptr;
		BaseIterator(IntrusiveListNode<T>* node);
		friend class IntrusiveList<T>;
	};

  public:
	NON_COPYABLE(IntrusiveList);
	using ConstIterator = BaseIterator<const T>;
	using Iterator = BaseIterator<T>;
	static_assert(validate_BidirectionalIterator<Iterator, ConstIterator>, "Not valid Bidirectional Iterator.");

	IntrusiveList();
	IntrusiveList(IntrusiveList&&);
	IntrusiveList& operator=(IntrusiveList&&);
	~IntrusiveList() = default;

	T& push_back(T& node);
	T& pop_front();
	T& remove(T& node);
	Iterator begin() const;
	Iterator end() const;
	ConstIterator cbegin() const;
	ConstIterator cend() const;
	T& first() const;
	T& last() const;
	Iterator find(T& node) const;
	bool is_empty() const;
	size_t size() const;
};
template <typename T>
IntrusiveList<T>::IntrusiveList() :
    m_count{0},
    m_head{&m_ending_node},
    m_tail{&m_ending_node},
    m_ending_node{&m_ending_node, &m_ending_node, this}
{
}

template <typename T>
IntrusiveList<T>::IntrusiveList(IntrusiveList<T>&& other) :
    m_count{other.m_count},
    m_head{other.m_head},
    m_tail{other.m_tail},
    m_ending_node{&m_ending_node, &m_ending_node, this}
{
	m_tail->next = &m_ending_node;
	other.m_head = other.m_tail = nullptr;
	other.m_count = 0;
}

template <typename T> IntrusiveList<T>& IntrusiveList<T>::operator=(IntrusiveList<T>&& other)
{
	if (&other != this) {
		m_count = other.m_count;
		m_head = other.m_head;
		m_tail = other.m_tail;
		m_tail->next = &m_ending_node;
		other.m_head = other.m_tail = nullptr;
		other.m_count = 0;
	}
	return *this;
}

template <typename T> T& IntrusiveList<T>::push_back(T& node)
{
	append_node(node);
	return node;
}

template <typename T> T& IntrusiveList<T>::pop_front()
{
	ASSERT(m_head != &m_ending_node);

	auto node = m_head;
	remove_node(*node);
	return *static_cast<T*>(node);
}

template <typename T> T& IntrusiveList<T>::remove(T& node)
{
	ASSERT(m_head);

	remove_node(node);
	return node;
}

template <typename T> typename IntrusiveList<T>::Iterator IntrusiveList<T>::begin() const
{
	return Iterator(m_head);
}

template <typename T> typename IntrusiveList<T>::Iterator IntrusiveList<T>::end() const
{
	return Iterator(&m_ending_node);
}

template <typename T> typename IntrusiveList<T>::ConstIterator IntrusiveList<T>::cbegin() const
{
	return ConstIterator(m_head);
}

template <typename T> typename IntrusiveList<T>::ConstIterator IntrusiveList<T>::cend() const
{
	return ConstIterator(&m_ending_node);
}

template <typename T> typename IntrusiveList<T>::Iterator IntrusiveList<T>::find(T& node) const
{
	if (node.owner == this) {
		return Iterator{&node};
	} else {
		return end();
	}
}

template <typename T> T& IntrusiveList<T>::first() const
{
	ASSERT(m_head != &m_ending_node);
	return *static_cast<T*>(m_head);
}

template <typename T> T& IntrusiveList<T>::last() const
{
	ASSERT(m_tail != &m_ending_node);
	return *static_cast<T*>(m_tail);
}

template <typename T> bool IntrusiveList<T>::is_empty() const
{
	return !m_count;
}

template <typename T> size_t IntrusiveList<T>::size() const
{
	return m_count;
}

template <typename T> void IntrusiveList<T>::remove_node(IntrusiveListNode<T>& node)
{
	ASSERT(is_valid_node(node));

	if ((m_head == &node) && (m_tail == &node)) {
		m_head = m_tail = &m_ending_node;
	} else if (m_head == &node) {
		m_head = node.next;
		node.next->prev = nullptr;
	} else if (m_tail == &node) {
		m_tail = node.prev;
		node.prev->next = &m_ending_node;
	} else {
		node.prev->next = node.next;
		node.next->prev = node.prev;
	}
	node.owner = nullptr;
	m_count--;
}

template <typename T> void IntrusiveList<T>::append_node(IntrusiveListNode<T>& node)
{
	ASSERT(node.owner == nullptr);

	node.owner = this;
	if (m_head == &m_ending_node) {
		node.prev = nullptr;
		node.next = &m_ending_node;
		m_head = m_tail = &node;
	} else {
		node.prev = m_tail;
		node.next = &m_ending_node;
		m_tail->next = &node;
		m_tail = &node;
		m_ending_node.prev = m_tail;
	}
	m_count++;
}

template <typename T> bool IntrusiveList<T>::is_valid_node(const IntrusiveListNode<T>& node) const
{
	if (node.next == &node) {
		return false;
	}
	if (node.owner != this) {
		return false;
	}
	return true;
}

template <typename T>
template <typename U>
IntrusiveList<T>::template BaseIterator<U>::BaseIterator(IntrusiveListNode<T>* node) : m_node{node}
{
	ASSERT(m_node);
}

template <typename T>
template <typename U>
IntrusiveList<T>::template BaseIterator<U>::BaseIterator(const BaseIterator<U>& other) : m_node{other.m_node}
{
	ASSERT(m_node);
}

template <typename T>
template <typename U>
typename IntrusiveList<T>::template BaseIterator<U>& IntrusiveList<T>::template BaseIterator<U>::operator=(
    const BaseIterator<U>& other)
{
	m_node = other.m_node;
	ASSERT(m_node);
	return *this;
}

template <typename T>
template <typename U>
IntrusiveList<T>::template BaseIterator<U>::operator BaseIterator<const RemoveConst<U>>()
{
	return BaseIterator<const RemoveConst<U>>{m_node};
}

template <typename T>
template <typename U>
bool IntrusiveList<T>::template BaseIterator<U>::operator==(const BaseIterator<U>& other) const
{
	return m_node == other.m_node;
}

template <typename T>
template <typename U>
bool IntrusiveList<T>::template BaseIterator<U>::operator!=(const BaseIterator<U>& other) const
{
	return m_node != other.m_node;
}

template <typename T>
template <typename U>
typename IntrusiveList<T>::template BaseIterator<U>& IntrusiveList<T>::template BaseIterator<U>::operator++()
{
	m_node = m_node->next;
	ASSERT(m_node);
	return *this;
}

template <typename T>
template <typename U>
typename IntrusiveList<T>::template BaseIterator<U> IntrusiveList<T>::template BaseIterator<U>::operator++(int)
{
	BaseIterator old(*this);
	m_node = m_node->next;
	ASSERT(m_node);
	return old;
}

template <typename T>
template <typename U>
typename IntrusiveList<T>::template BaseIterator<U>& IntrusiveList<T>::template BaseIterator<U>::operator--()
{
	m_node = m_node->prev;
	ASSERT(m_node);
	return *this;
}

template <typename T>
template <typename U>
typename IntrusiveList<T>::template BaseIterator<U> IntrusiveList<T>::template BaseIterator<U>::operator--(int)
{
	BaseIterator old(*this);
	m_node = m_node->prev;
	ASSERT(m_node);
	return old;
}

template <typename T> template <typename U> U& IntrusiveList<T>::template BaseIterator<U>::operator*()
{
	ASSERT(m_node->owner);
	ASSERT(m_node->owner->is_valid_node(*m_node));
	return *static_cast<U*>(m_node);
}

template <typename T> template <typename U> U* IntrusiveList<T>::template BaseIterator<U>::operator->()
{
	ASSERT(m_node->owner);
	ASSERT(m_node->owner->is_valid_node(*m_node));
	return static_cast<U*>(m_node);
}