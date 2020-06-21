#pragma once
#include "assert.h"

template <class T> class CircularList
{
  private:
	struct Node {
		T data;
		Node *next, *prev;
	};
	Node* m_head;
	void unlink_node(Node* node);
	void link_node(Node* new_node, Node* pnode);

  public:
	class Iterator
	{
	  private:
		Node* m_current;
		CircularList<T>* current_list;

	  public:
		Iterator(CircularList<T>* _list);
		Node* operator++(int);
		Node* operator--(int);
		Node* node();
		void set_cursor(int index);
		bool is_head();
	};
	CircularList();
	~CircularList();
	void push_back(const T& new_data);
	void push_front(const T& new_data);
	void pop_back();
	void pop_front();
	void remove(Iterator&);
	void remove(int index);
	void increment_head();
	void set_head(Iterator&);
	void set_head(int index);
	void move_to_other_list(CircularList<T>* list, Iterator& itr);
	void move_head_to_other_list(CircularList<T>* list);
	bool is_empty();
	T& head_data();
	T& data(Iterator&);
	T& operator[](int index);
};

template <class T> CircularList<T>::Iterator::Iterator(CircularList<T>* _list)
{
	current_list = _list;
	m_current = current_list->m_head;
}

// Set current node pointer to an input index relative to the head.
template <class T> void CircularList<T>::Iterator::set_cursor(int index)
{
	Node* p = current_list->m_head;
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
template <class T> typename CircularList<T>::Node* CircularList<T>::Iterator::operator++(int)
{
	m_current = m_current->next;
	return m_current;
}

// Decrement the current node pointer.
template <class T> typename CircularList<T>::Node* CircularList<T>::Iterator::operator--(int)
{
	m_current = m_current->prev;
	return m_current;
}

// Get node pointer.
template <class T> typename CircularList<T>::Node* CircularList<T>::Iterator::node()
{
	return m_current;
}

// Check if the current iteration is the head.
template <class T> bool CircularList<T>::Iterator::is_head()
{
	if (m_current == current_list->m_head) {
		return true;
	} else {
		return false;
	}
}

template <class T> CircularList<T>::CircularList()
{
	m_head = nullptr;
}

template <class T> CircularList<T>::~CircularList()
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
template <class T> void CircularList<T>::move_to_other_list(CircularList<T>* list, Iterator& itr)
{
	ASSERT(list);
	unlink_node(itr.node());
	list->link_node(itr.node(), list->m_head);
}

// Move head node to other list.
template <class T> void CircularList<T>::move_head_to_other_list(CircularList<T>* list)
{
	ASSERT(list);
	Node* node = m_head;
	unlink_node(node);
	list->link_node(node, list->m_head);
}

// unlink node from the list.
template <class T> void CircularList<T>::unlink_node(Node* node)
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
}

// link a new node before `pnode`.
template <class T> void CircularList<T>::link_node(Node* new_node, Node* pnode)
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
}

// Push data to the back of the list.
template <class T> void CircularList<T>::push_back(const T& new_data)
{
	Node* new_node = new Node;
	new_node->data = new_data;
	link_node(new_node, m_head);
}

// Push data to the front of the list.
template <class T> void CircularList<T>::push_front(const T& new_data)
{
	push_back(new_data);
	m_head = m_head->prev;
}

// The second node will be the head.
template <class T> void CircularList<T>::increment_head()
{
	ASSERT(m_head);
	m_head = m_head->next;
}

// Select the head node using Iterator.
template <class T> void CircularList<T>::set_head(Iterator& itr)
{
	ASSERT(m_head);
	m_head = itr.node();
}

// Select the head node using an index.
template <class T> void CircularList<T>::set_head(int index)
{
	ASSERT(m_head);
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	m_head = itr.node();
}

// Remove the last node.
template <class T> void CircularList<T>::pop_back()
{
	ASSERT(m_head);
	Node* node = m_head->prev;
	unlink_node(node);
	delete node;
}

// Remove the first node.
template <class T> void CircularList<T>::pop_front()
{
	ASSERT(m_head);
	Node* node = m_head;
	unlink_node(node);
	delete node;
}

// Remove a node whose selected by Iterator.
template <class T> void CircularList<T>::remove(Iterator& itr)
{

	ASSERT(m_head);
	Node* node = itr.node();
	unlink_node(node);
	delete node;
}

// Remove a node whose selected by its index.
template <class T> void CircularList<T>::remove(int index)
{
	ASSERT(m_head);
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	remove(itr);
}

// Get data of the node selected by iterator.
template <class T> T& CircularList<T>::data(Iterator& itr)
{
	ASSERT(m_head);
	return itr.node()->data;
}

// Get data of the head node.
template <class T> T& CircularList<T>::head_data()
{
	ASSERT(m_head);
	return m_head->data;
}

template <class T> T& CircularList<T>::operator[](int index)
{
	ASSERT(m_head);
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	return itr.node().data;
}

template <class T> bool CircularList<T>::is_empty()
{
	if (m_head)
		return false;
	else
		return true;
}