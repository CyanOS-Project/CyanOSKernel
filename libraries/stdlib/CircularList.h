#pragma once

#include <Algorithms.h>
#include <Types.h>
#ifdef __UNIT_TESTS
	#include <assert.h>
	#define ASSERT(x) assert(x)
#else
	#include <Assert.h>
#endif

template <class T> class CircularQueue
{
  private:
	struct Node {
		T data;
		Node *next, *prev;
	};
	Node* m_head;
	size_t m_count;
	void unlink_node(Node* node);
	void link_node(Node* new_node, Node* pnode);

  public:
	class Iterator
	{
	  private:
		Node* m_current;
		Node* m_head;

	  public:
		Iterator(Node* t_list);
		Node* node();
		void move_cursor(int index);
		Iterator& operator++(int);
		Iterator& operator++();
		bool operator!=(const CircularQueue<T>::Iterator& other);
		bool operator==(const CircularQueue<T>::Iterator& other);
		void operator=(const CircularQueue<T>::Iterator& other);
		T* operator->();
		T& operator*();
	};
	CircularQueue();
	~CircularQueue();
	Iterator begin();
	Iterator end();
	template <typename... U> T& emplace_back(U&&... u);
	template <typename... U> T& emplace_front(U&&... u);
	T& push_back(const T& new_data);
	T& push_front(const T& new_data);
	T& push_back(T&& new_data);
	T& push_front(T&& new_data);
	void pop_back();
	void pop_front();
	void remove(Iterator&);
	void remove(int index);
	void increment_head();
	void set_head(Iterator&);
	void set_head(int index);
	void move_to_other_list(CircularQueue<T>* list, Iterator& itr);
	void move_head_to_other_list(CircularQueue<T>* list);
	bool is_empty();
	size_t size();
	T& head();
	T& operator[](int index);
};

template <class T> CircularQueue<T>::Iterator::Iterator(Node* t_node) : m_current(t_node), m_head(t_node) {}

// move the current node pointer number of nodes forward or backward relative to the current head.
template <class T> void CircularQueue<T>::Iterator::move_cursor(int index)
{
	Node* p = m_head;
	if (index > 0) {
		while (index--) {
			p = p->next;
		}
	} else {
		while (index++) {
			p = p->prev;
		}
	}
	m_current = p;
}

// Increment the current node pointer.
template <class T> typename CircularQueue<T>::Iterator& CircularQueue<T>::Iterator::operator++(int arg)
{
	const auto next = m_current->next;
	if (next == m_head) {
		m_current = nullptr; // iterated through the whole list.
	} else {
		m_current = next;
	}
	return *this;
}

// Increment the current node pointer.
template <class T> typename CircularQueue<T>::Iterator& CircularQueue<T>::Iterator::operator++()
{
	const auto next = m_current->next;
	if (next == m_head) {
		m_current = nullptr; // iterated through the whole list.
	} else {
		m_current = next;
	}
	return *this;
}

template <class T> bool CircularQueue<T>::Iterator::operator!=(const CircularQueue<T>::Iterator& other)
{
	return m_current != other.m_current;
}

template <class T> bool CircularQueue<T>::Iterator::operator==(const CircularQueue<T>::Iterator& other)
{
	return m_current == other.m_current;
}

template <class T> void CircularQueue<T>::Iterator::operator=(const CircularQueue<T>::Iterator& other)
{
	m_current = other->m_current;
	m_head = other->m_head;
}

template <class T> T& CircularQueue<T>::Iterator::operator*()
{
	return m_current->data;
}

template <class T> T* CircularQueue<T>::Iterator::operator->()
{
	return &m_current->data;
}

// Get node pointer.
template <class T> typename CircularQueue<T>::Node* CircularQueue<T>::Iterator::node()
{
	return m_current;
}

template <class T> CircularQueue<T>::CircularQueue() : m_head(nullptr), m_count(0) {}

template <class T> CircularQueue<T>::~CircularQueue()
{
	if (!m_head)
		return;

	Node* node_iterator = m_head;
	do {
		Node* next = node_iterator->next;
		delete node_iterator;
		node_iterator = next;
	} while (node_iterator != m_head);
}

// Move node to other list.
template <class T> void CircularQueue<T>::move_to_other_list(CircularQueue<T>* list, Iterator& itr)
{
	ASSERT(list);
	unlink_node(itr.node());
	list->link_node(itr.node(), list->m_head);
}

// Move head node to other list.
template <class T> void CircularQueue<T>::move_head_to_other_list(CircularQueue<T>* list)
{
	ASSERT(list);
	Node* node = m_head;
	unlink_node(node);
	list->link_node(node, list->m_head);
}

// unlink node from the list.
template <class T> void CircularQueue<T>::unlink_node(Node* node)
{
	ASSERT(m_head);
	ASSERT(node);
	if (node == node->next) { // remove the last node
		m_head = nullptr;
	} else {
		if (node == m_head) {
			m_head = node->next;
		}
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	m_count--;
}

// link a new node before `pnode`.
template <class T> void CircularQueue<T>::link_node(Node* new_node, Node* pnode)
{
	ASSERT(new_node);
	if (pnode) {
		new_node->next = pnode;
		new_node->prev = pnode->prev;
		pnode->prev->next = new_node;
		pnode->prev = new_node;
	} else {
		new_node->next = new_node->prev = new_node;
	}
	if (!m_head) {
		m_head = new_node;
	}
	m_count++;
}

// Get iterator object for the head node.
template <class T> typename CircularQueue<T>::Iterator CircularQueue<T>::begin()
{
	return Iterator(m_head);
}

// Get iterator object for the last node.
template <class T> typename CircularQueue<T>::Iterator CircularQueue<T>::end()
{
	return Iterator(nullptr);
}

template <class T> template <typename... U> T& CircularQueue<T>::emplace_back(U&&... u)
{
	Node* new_node = new Node{T{forward<U>(u)...}, nullptr, nullptr};
	link_node(new_node, m_head);
	return new_node->data;
}

template <class T> template <typename... U> T& CircularQueue<T>::emplace_front(U&&... u)
{
	Node* new_node = new Node{T{forward<U>(u)...}, nullptr, nullptr};
	link_node(new_node, m_head);
	m_head = m_head->prev;
	return new_node->data;
}

// Push data to the back of the list.
template <class T> T& CircularQueue<T>::push_back(const T& new_data)
{
	Node* new_node = new Node{new_data, nullptr, nullptr};
	link_node(new_node, m_head);
	return new_node->data;
}

template <class T> T& CircularQueue<T>::push_front(const T& new_data)
{
	T& new_node_data = push_back(new_data);
	m_head = m_head->prev;
	return new_node_data;
}

// Push data to the back of the list.
template <class T> T& CircularQueue<T>::push_back(T&& new_data)
{
	Node* new_node = new Node{move(new_data), nullptr, nullptr};
	link_node(new_node, m_head);
	return new_node->data;
}

template <class T> T& CircularQueue<T>::push_front(T&& new_data)
{
	T& new_node_data = push_back(move(new_data));
	m_head = m_head->prev;
	return new_node_data;
}

// The second node will be the head.
template <class T> void CircularQueue<T>::increment_head()
{
	ASSERT(m_head);
	m_head = m_head->next;
}

// Select the head node using Iterator.
template <class T> void CircularQueue<T>::set_head(Iterator& itr)
{
	ASSERT(m_head);
	m_head = itr.node();
}

// Select the head node using an index.
template <class T> void CircularQueue<T>::set_head(int index)
{
	ASSERT(m_head);
	Iterator itr(m_head);
	itr.move_cursor(index);
	m_head = itr.node();
}

// Remove the last node.
template <class T> void CircularQueue<T>::pop_back()
{
	ASSERT(m_head);
	Node* node = m_head->prev;
	unlink_node(node);
	delete node;
}

// Remove the first node.
template <class T> void CircularQueue<T>::pop_front()
{
	ASSERT(m_head);
	Node* node = m_head;
	unlink_node(node);
	delete node;
}

// Remove a node whose selected by Iterator.
template <class T> void CircularQueue<T>::remove(Iterator& itr)
{

	ASSERT(m_head);
	Node* node = itr.node();
	unlink_node(node);
	delete node;
}

// Remove a node whose selected by its index.
template <class T> void CircularQueue<T>::remove(int index)
{
	ASSERT(m_head);
	Iterator itr(m_head);
	itr.move_cursor(index);
	remove(itr);
}

// Get data of the head node.
template <class T> T& CircularQueue<T>::head()
{
	ASSERT(m_head);
	return m_head->data;
}

template <class T> T& CircularQueue<T>::operator[](int index)
{
	ASSERT(m_head);
	Iterator itr(m_head);
	itr.move_cursor(index);
	return *itr;
}

template <class T> bool CircularQueue<T>::is_empty()
{
	if (m_count)
		return false;
	else
		return true;
}

template <class T> size_t CircularQueue<T>::size()
{
	return m_count;
}