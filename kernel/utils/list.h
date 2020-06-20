#include "Devices/Console/console.h"

template <class T> class CircularList
{
  private:
	struct Node {
		T data;
		Node *next, *prev;
	};
	Node* m_first;
	void unlink_node(Node* node);

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
	void remove(const Iterator&);
	void remove(int index);
	void increment_head();
	void set_head(const Iterator&);
	void set_head(int index);
	void move_to_list(const CircularList<T>* list);
	T& head_data();
	T& data(Iterator&);
	T& operator[](int index);
};

template <class T> CircularList<T>::Iterator::Iterator(CircularList<T>* _list)
{
	current_list = _list;
	m_current = current_list->m_first;
}

// Set current node pointer to an input index relative to the head.
template <class T> void CircularList<T>::Iterator::set_cursor(int index)
{
	Node* p = current_list->m_first;
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
	if (m_current == current_list->m_first) {
		return true;
	} else {
		return false;
	}
}

/*template <class T> void CircularList<T>::move_to_list(const CircularList<T> list)
{
}*/

template <class T> CircularList<T>::CircularList()
{
	m_first = nullptr;
}

template <class T> CircularList<T>::~CircularList()
{
	Node* node_iterator = m_first;
	do {
		Node* next = node_iterator->next;
		delete node_iterator;
		node_iterator = next;
	} while (node_iterator != m_first);
}

// unlink node from the list.
template <class T> void CircularList<T>::unlink_node(Node* node)
{
	if (node == node->next) { // remove the last node
		m_first = nullptr;
	} else {
		if (node == m_first) {
			m_first = node->next;
		}
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
}

// Push data to the back of the list.
template <class T> void CircularList<T>::push_back(const T& new_data)
{
	Node* new_node = new Node;
	if (m_first) {
		new_node->next = m_first;
		new_node->prev = m_first->prev;
		m_first->prev->next = new_node;
		m_first->prev = new_node;
		new_node->data = new_data;
	} else {
		new_node->data = new_data;
		m_first = new_node->next = new_node->prev = new_node;
	}
}

// Push data to the front of the list.
template <class T> void CircularList<T>::push_front(const T& new_data)
{
	push_back(new_data);
	m_first = m_first->prev;
}

// The second node will be the head.
template <class T> void CircularList<T>::increment_head()
{
	m_first = m_first->next;
}

// Select the head node using Iterator.
template <class T> void CircularList<T>::set_head(const Iterator& itr)
{
	m_first = itr.node();
}

// Select the head node using an index.
template <class T> void CircularList<T>::set_head(int index)
{
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	m_first = itr.node();
}

// Remove the last node.
template <class T> void CircularList<T>::pop_back()
{
	unlink_node(m_first->prev);
}

// Remove the first node.
template <class T> void CircularList<T>::pop_front()
{
	unlink_node(m_first);
}

// Remove a node whose selected by Iterator.
template <class T> void CircularList<T>::remove(const Iterator& itr)
{
	unlink_node(itr.node());
}

// Remove a node whose selected by its index.
template <class T> void CircularList<T>::remove(int index)
{
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	unlink_node(itr.node());
}

// Get data of the node selected by iterator.
template <class T> T& CircularList<T>::data(Iterator& itr)
{
	return itr.node()->data;
}

// Get data of the head node.
template <class T> T& CircularList<T>::head_data()
{
	return m_first->data;
}

template <class T> T& CircularList<T>::operator[](int index)
{
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	return itr.node().data;
}
