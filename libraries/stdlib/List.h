#pragma once

#include "Algorithms.h"
#include "Concepts.h"
#include "Rule5.h"
#include "Types.h"

#ifdef __UNIT_TESTS
	#include <assert.h>
	#define ASSERT(x) assert(x)
#else
	#include "Assert.h"
#endif

template <class T> class List
{
  private:
	struct Node;
	struct ShallowNode {
		Node *next, *prev;
		ShallowNode() : next{nullptr}, prev{nullptr} {}
	};
	struct Node : ShallowNode {
		T data;
		Node(const T& d) : data{d} {}
		Node(T&& d) : data{move(d)} {}
		template <typename... U> Node(U&&... u) : data{forward<U>(u)...} {}
	};
	mutable ShallowNode m_shallow_ending_node;
	Node* const m_ending_node = static_cast<Node*>(&m_shallow_ending_node);
	Node* m_head;
	Node* m_tail;
	size_t m_count;
	void unlink_node(Node& new_node);
	void link_node(Node& new_node, Node& node);

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
		T& operator*();
		T* operator->();

	  private:
		Node* m_node;
		List<T>* owner;
		BaseIterator(List<T>::Node* node);

		friend List<T>;
	};

  public:
	using ConstIterator = BaseIterator<const T>;
	using Iterator = BaseIterator<T>;
	static_assert(validate_BidirectionalIterator<Iterator, ConstIterator>, "Not valid Bidirectional Iterator.");

	List();
	List(const List&);
	List(List&&);
	List& operator=(const List&);
	List& operator=(List&&);
	~List();
	Iterator begin();
	Iterator end();
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
	void remove(Iterator);
	template <class Predicate> bool remove_if(Predicate predicate);
	void splice(List<T>& list, Iterator itr);
	ConstIterator find(const T& element) const;
	Iterator find(const T& element);
	template <class Predicate> ConstIterator find_if(Predicate predicate) const;
	template <class Predicate> Iterator find_if(Predicate predicate);
	template <class Predicate> bool contains(Predicate predicate);
	void clear();
	bool is_empty() const;
	size_t size() const;
	T& first();
	const T& first() const;
	T& last();
	const T& last() const;
	T& operator[](int);
};

template <class T> List<T>::List() : m_shallow_ending_node{}, m_head{m_ending_node}, m_tail{m_ending_node}, m_count{0}
{
}

template <class T>
List<T>::List(const List<T>& other) : m_shallow_ending_node{}, m_head{m_ending_node}, m_tail{m_ending_node}, m_count{0}
{
	for (auto i = other.cbegin(); i != other.cend(); ++i) {
		push_back(*i);
	}
}

template <class T>
List<T>::List(List<T>&& other) :
    m_shallow_ending_node{},
    m_head{other.m_head},
    m_tail{other.m_tail},
    m_count{other.m_count}
{
	m_tail->next = m_ending_node;
	other.m_head = other.m_tail = nullptr;
	other.m_count = 0;
}

template <class T> List<T>& List<T>::operator=(const List<T>& other)
{
	if (&other != this) {
		clear();
		for (auto i = other.cbegin(); i != other.cend(); ++i) {
			push_back(*i);
		}
	}
	return *this;
}

template <class T> List<T>& List<T>::operator=(List<T>&& other)
{
	if (&other != this) {
		clear();
		m_head = other.m_head;
		m_tail = other.m_tail;
		m_count = other.m_count;
		m_tail->next = m_ending_node;

		other.m_head = other.m_tail = nullptr;
		other.m_count = 0;
	}
	return *this;
}

template <class T> List<T>::~List()
{
	clear();
}

template <class T> typename List<T>::Iterator List<T>::begin()
{
	return Iterator(m_head);
}

template <class T> typename List<T>::Iterator List<T>::end()
{
	return Iterator(m_ending_node);
}

template <class T> typename List<T>::ConstIterator List<T>::cbegin() const
{
	return ConstIterator(m_head);
}

template <class T> typename List<T>::ConstIterator List<T>::cend() const
{
	return ConstIterator(m_ending_node);
}

template <class T> template <typename... U> T& List<T>::emplace(Iterator itr, U&&... u)
{
	Node* new_node = new Node{forward<U>(u)...};
	link_node(*new_node, *itr.m_node);
	return new_node->data;
}

template <class T> template <typename U> T& List<T>::insert(Iterator pos, U&& new_data)
{
	Node* new_node = new Node{forward<U>(new_data)};
	link_node(*new_node, *pos.m_node);
	return new_node->data;
}

template <class T> template <typename... U> T& List<T>::emplace_back(U&&... u)
{
	return emplace(end(), forward<U>(u)...);
}

template <class T> template <typename... U> T& List<T>::emplace_front(U&&... u)
{
	return emplace(begin(), forward<U>(u)...);
}

template <class T> template <typename U> T& List<T>::push_back(U&& new_data)
{
	return insert(end(), forward<U>(new_data));
}

template <class T> template <typename U> T& List<T>::push_front(U&& new_data)
{
	return insert(begin(), forward<U>(new_data));
}

template <class T> void List<T>::remove(Iterator itr)
{
	ASSERT(itr.m_node);
	unlink_node(*itr.m_node);
	delete itr.m_node;
}

template <class T> void List<T>::pop_back()
{
	remove(--end());
}

template <class T> void List<T>::pop_front()
{
	remove(begin());
}

template <class T> template <class Predicate> bool List<T>::remove_if(Predicate predicate)
{
	bool is_removed = false;
	auto&& i = begin();
	while (i != end()) {
		auto iterator_copy = i++;
		if (predicate(*iterator_copy)) {
			remove(iterator_copy);
			is_removed = true;
		}
	}
	return is_removed;
}

template <class T> void List<T>::splice(List<T>& list, const Iterator itr)
{
	unlink_node(*itr.m_node);
	list.link_node(*itr.m_node, *list.m_ending_node);
}

template <class T> typename List<T>::Iterator List<T>::find(const T& element)
{
	for (auto i = begin(); i != end(); i++) {
		if (element == *i) {
			return i;
		}
	}
	return end();
}

template <class T> template <class Predicate> typename List<T>::Iterator List<T>::find_if(Predicate predicate)
{
	for (auto i = begin(); i != end(); i++) {
		if (predicate(*i)) {
			return i;
		}
	}
	return end();
}

template <class T> template <class Predicate> bool List<T>::contains(Predicate predicate)
{
	for (const auto& i : *this) {
		if (predicate(i)) {
			return true;
		}
	}
	return false;
}

template <class T> T& List<T>::first()
{
	return *begin();
}

template <class T> T& List<T>::last()
{
	return *(--end());
}

template <class T> const T& List<T>::first() const
{
	return *begin();
}

template <class T> const T& List<T>::last() const
{
	return *(--end());
}

template <class T> bool List<T>::is_empty() const
{
	return !size();
}

template <class T> size_t List<T>::size() const
{
	return m_count;
}

template <class T> void List<T>::clear()
{
	Node* curr = m_head;
	while (curr && curr != m_ending_node) {
		Node* next = curr->next;
		delete curr;
		curr = next;
	}
	m_tail = m_head = m_ending_node;
	m_count = 0;
}

// FIXME: Delete this operator!!

template <class T> T& List<T>::operator[](int num)
{
	auto itr = begin();
	while (num--) {
		++itr;
	}
	return *itr;
}

template <class T> void List<T>::unlink_node(Node& node)
{
	if ((m_head == &node) && (m_tail == &node)) {
		m_head = m_tail = m_ending_node;
	} else if (m_head == &node) {
		m_head = node.next;
		node.next->prev = nullptr;
	} else if (m_tail == &node) {
		m_tail = node.prev;
		node.prev->next = m_ending_node;
		m_ending_node->prev = node.prev;
	} else {
		node.prev->next = node.next;
		node.next->prev = node.prev;
	}
	m_count--;
}

template <class T> void List<T>::link_node(Node& new_node, Node& old_node)
{
	if (m_count == 0) {
		new_node.prev = nullptr;
		new_node.next = m_ending_node;
		m_ending_node->prev = &new_node;
		m_head = m_tail = &new_node;
	} else if (&old_node == m_head) {
		new_node.prev = nullptr;
		new_node.next = m_head;
		m_head->prev = &new_node;
		m_head = &new_node;
	} else if (&old_node == m_ending_node) {
		new_node.prev = m_ending_node->prev;
		new_node.next = m_ending_node;
		m_ending_node->prev->next = &new_node;
		m_ending_node->prev = &new_node;
		m_tail = &new_node;
	} else {
		new_node.prev = old_node.prev;
		new_node.next = &old_node;
		old_node.prev->next = &new_node;
		old_node.prev = &new_node;
	}
	m_count++;
}

template <typename T>
template <typename U>
List<T>::BaseIterator<U>::BaseIterator(typename List<T>::Node* node) : m_node{node}
{
	ASSERT(m_node);
}

template <typename T>
template <typename U>
List<T>::BaseIterator<U>::BaseIterator(const BaseIterator<U>& other) : m_node{other.m_node}
{
	ASSERT(m_node);
}

template <typename T>
template <typename U>
typename List<T>::BaseIterator<U>& List<T>::BaseIterator<U>::operator=(const BaseIterator<U>& other)
{
	m_node = other.m_node;
	ASSERT(m_node);
	return *this;
}

template <typename T> template <typename U> List<T>::BaseIterator<U>::operator BaseIterator<const RemoveConst<U>>()
{
	return BaseIterator<const RemoveConst<U>>{m_node};
}

template <typename T>
template <typename U>
bool List<T>::BaseIterator<U>::operator==(const BaseIterator<U>& other) const
{
	return m_node == other.m_node;
}

template <typename T>
template <typename U>
bool List<T>::BaseIterator<U>::operator!=(const BaseIterator<U>& other) const
{
	return m_node != other.m_node;
}

template <typename T> template <typename U> typename List<T>::BaseIterator<U>& List<T>::BaseIterator<U>::operator++()
{
	m_node = m_node->next;
	ASSERT(m_node);
	return *this;
}

template <typename T> template <typename U> typename List<T>::BaseIterator<U> List<T>::BaseIterator<U>::operator++(int)
{
	BaseIterator old(*this);
	m_node = m_node->next;
	ASSERT(m_node);
	return old;
}

template <typename T> template <typename U> typename List<T>::BaseIterator<U>& List<T>::BaseIterator<U>::operator--()
{
	m_node = m_node->prev;
	ASSERT(m_node);
	return *this;
}

template <typename T> template <typename U> typename List<T>::BaseIterator<U> List<T>::BaseIterator<U>::operator--(int)
{
	BaseIterator old(*this);
	m_node = m_node->prev;
	ASSERT(m_node);
	return old;
}

template <typename T> template <typename U> T& List<T>::BaseIterator<U>::operator*()
{
	// ASSERT(m_node->owner);
	// ASSERT(m_node->owner->is_valid_node(*m_node));
	return m_node->data;
}

template <typename T> template <typename U> T* List<T>::BaseIterator<U>::operator->()
{
	// ASSERT(m_node->owner);
	// ASSERT(m_node->owner->is_valid_node(*m_node));
	return &m_node->data;
}