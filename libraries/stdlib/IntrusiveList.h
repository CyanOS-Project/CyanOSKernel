#pragma once

#include "Algorithms.h"
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
	T* next = nullptr;
	T* prev = nullptr;
	IntrusiveList<T>* owner = nullptr;
	friend class IntrusiveList<T>;
};

template <typename T> class IntrusiveList
{
  private:
	T* m_head = nullptr;
	T* m_tail = nullptr;
	size_t m_count = 0;

	void remove_node(T& node);
	void append_node(T& node);

	template <typename Type> class BaseIterator
	{
	  public:
		BaseIterator(T* node);
		BaseIterator(const BaseIterator& other);
		operator BaseIterator<const RemoveConst<Type>>();
		~BaseIterator() = default;
		bool operator==(const BaseIterator& other) const;
		bool operator!=(const BaseIterator& other) const;
		BaseIterator& operator++();
		BaseIterator operator++(int);
		BaseIterator& operator--();
		BaseIterator operator--(int);
		BaseIterator operator+(int) const;
		BaseIterator operator-(int) const;
		Type& operator*();
		Type* operator->();

	  private:
		T* m_node = nullptr;
	};

  public:
	NON_COPYABLE(IntrusiveList);
	NON_MOVABLE(IntrusiveList);
	using ConstIterator = BaseIterator<const T>;
	using Iterator = BaseIterator<T>;

	IntrusiveList() = default;
	~IntrusiveList() = default;

	T& push_back(T& node);
	T& pop_front();
	T& remove(T& node);
	Iterator begin() const;
	Iterator end() const;
	ConstIterator cbegin() const;
	ConstIterator cend() const;
	T& first() const;
	T& tail() const;
	Iterator find(T& node) const;
	bool is_empty() const;
	size_t size() const;
};

template <typename T> void IntrusiveList<T>::remove_node(T& node)
{
	ASSERT(node.owner == this);
	if ((m_head == &node) && (m_tail == &node)) {
		m_head = m_tail = nullptr;
	} else if (m_head == &node) {
		m_head = node.next;
		node.next->prev = nullptr;
	} else if (m_tail == &node) {
		m_tail = node.prev;
		node.prev->next = nullptr;
	} else {
		node.prev->next = node.next;
		node.next->prev = node.prev;
	}
	node.owner = nullptr;
	m_count--;
}

template <typename T> void IntrusiveList<T>::append_node(T& node)
{
	if (node.owner == nullptr)
		node.owner = this;

	if (!m_head) {
		node.prev = nullptr;
		node.next = nullptr;
		m_head = m_tail = &node;
	} else {
		node.prev = m_tail;
		node.next = nullptr;
		m_tail->next = &node;
		m_tail = &node;
	}
	m_count++;
}

template <typename T> T& IntrusiveList<T>::push_back(T& node)
{
	append_node(node);
	return node;
}

template <typename T> T& IntrusiveList<T>::pop_front()
{
	ASSERT(m_head);

	T* node = m_head;
	remove_node(*node);
	return *node;
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
	return Iterator(nullptr);
}

template <typename T> typename IntrusiveList<T>::ConstIterator IntrusiveList<T>::cbegin() const
{
	return ConstIterator(m_head);
}

template <typename T> typename IntrusiveList<T>::ConstIterator IntrusiveList<T>::cend() const
{
	return ConstIterator(nullptr);
}

template <typename T> typename IntrusiveList<T>::Iterator IntrusiveList<T>::find(T& node) const
{
	return Iterator{&node};
}

template <typename T> T& IntrusiveList<T>::first() const
{
	ASSERT(m_head);

	return *m_head;
}

template <typename T> T& IntrusiveList<T>::tail() const
{
	ASSERT(m_head);

	return *m_tail;
}

template <typename T> bool IntrusiveList<T>::is_empty() const
{
	return !m_count;
}

template <typename T> size_t IntrusiveList<T>::size() const
{
	return m_count;
}

template <typename T>
template <typename P>
IntrusiveList<T>::template BaseIterator<P>::BaseIterator(T* node) : m_node{node}
{
}

template <typename T>
template <typename P>
IntrusiveList<T>::template BaseIterator<P>::BaseIterator(const BaseIterator<P>& other) : m_node{other.m_node}
{
}
template <typename T>
template <typename P>
IntrusiveList<T>::template BaseIterator<P>::operator BaseIterator<const RemoveConst<P>>()
{
	return BaseIterator<const RemoveConst<P>>{m_node};
}

template <typename T>
template <typename P>
bool IntrusiveList<T>::template BaseIterator<P>::operator==(const BaseIterator<P>& other) const
{
	return m_node == other.m_node;
}

template <typename T>
template <typename P>
bool IntrusiveList<T>::template BaseIterator<P>::operator!=(const BaseIterator<P>& other) const
{
	return m_node != other.m_node;
}

template <typename T>
template <typename P>
typename IntrusiveList<T>::template BaseIterator<P>& IntrusiveList<T>::template BaseIterator<P>::operator++()
{
	if (m_node) {
		m_node = m_node->next;
		return *this;
	} else {
		return *this;
	}
}

template <typename T>
template <typename P>
typename IntrusiveList<T>::template BaseIterator<P> IntrusiveList<T>::template BaseIterator<P>::operator++(int)
{
	if (m_node) {
		BaseIterator old(*this);
		m_node = m_node->next;
		return old;
	} else {
		return *this;
	}
}

template <typename T>
template <typename P>
typename IntrusiveList<T>::template BaseIterator<P>& IntrusiveList<T>::template BaseIterator<P>::operator--()
{
	if (m_node) {
		m_node = m_node->prev;
		return *this;
	} else {
		return *this;
	}
}

template <typename T>
template <typename P>
typename IntrusiveList<T>::template BaseIterator<P> IntrusiveList<T>::template BaseIterator<P>::operator--(int)
{
	if (m_node) {
		BaseIterator old(*this);
		m_node = m_node->prev;
		return old;
	} else {
		return *this;
	}
}

template <typename T>
template <typename P>
typename IntrusiveList<T>::template BaseIterator<P> IntrusiveList<T>::template BaseIterator<P>::operator+(int num) const
{
	if (num == 0) {
		return *this;
	} else {
		auto curr = m_node;
		if (num > 0) {
			while (curr && num) {
				curr = curr->next;
				num--;
			}
		} else {
			while (curr && num) {
				curr = curr->prev;
				num++;
			}
		}
		if (!num) {
			return BaseIterator(curr);
		} else {
			return BaseIterator(nullptr);
		}
	}
}

template <typename T>
template <typename P>
typename IntrusiveList<T>::template BaseIterator<P> IntrusiveList<T>::template BaseIterator<P>::operator-(int num) const
{
	return operator+(-num);
}

template <typename T> template <typename P> P& IntrusiveList<T>::template BaseIterator<P>::operator*()
{
	ASSERT(m_node);
	return *m_node;
}

template <typename T> template <typename P> P* IntrusiveList<T>::template BaseIterator<P>::operator->()
{
	ASSERT(m_node);
	return m_node;
}