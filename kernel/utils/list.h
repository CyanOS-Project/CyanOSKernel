#include "Devices/Console/console.h"

template <class T> class CircularList
{
  private:
	struct Node {
		T data;
		Node *next, *prev;
	};
	Node* m_first;

  public:
	class Iterator
	{
	  private:
		CircularList<T>* current_list;
		Node* m_current;
		void unlink_node(Node* node);

	  public:
		Iterator(CircularList<T>* _list);
		T& operator++(int index);
		T& get_current();
		void set_cursor(int index);
		void remove_current();
		void move_to_list(CircularList<T> list);
		bool is_first();
	};
	CircularList();
	~CircularList();
	void push_back(T new_data);
	void push_front(T new_data);
	void pop_back();
	void pop_front();
	void remove(int index);
	T& operator[](int index);
};

template <class T> void CircularList<T>::Iterator::unlink_node(Node* node)
{
	if (node == node->next) { // remove the last node
		current_list->m_first = nullptr;
	} else {
		if (node == current_list->m_first) {
			current_list->m_first = node->next;
		}
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
}

template <class T> CircularList<T>::Iterator::Iterator(CircularList<T>* _list)
{
	current_list = _list;
	m_current = current_list->m_first;
}

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

template <class T> void CircularList<T>::Iterator::remove_current()
{
	unlink_node(m_current);
	Heap::kfree(m_current);
	m_current = current_list->m_first;
}

template <class T> T& CircularList<T>::Iterator::operator++(int index)
{
	m_current = m_current->next;
	return m_current->data;
}

template <class T> T& CircularList<T>::Iterator::get_current()
{
	return m_current->data;
}

template <class T> bool CircularList<T>::Iterator::is_first()
{
	if (m_current == current_list->m_first) {
		return true;
	} else {
		return false;
	}
}

template <class T> void CircularList<T>::Iterator::move_to_list(CircularList<T> list)
{
}

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

template <class T> void CircularList<T>::push_back(T new_data)
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

template <class T> void CircularList<T>::push_front(T new_data)
{
	push_back(new_data);
	m_first = m_first->prev;
}

template <class T> void CircularList<T>::pop_back()
{
	Iterator itr = Iterator(this);
	itr.set_cursor(-1);
	itr.remove_current();
}

template <class T> void CircularList<T>::pop_front()
{
	Iterator itr = Iterator(this);
	itr.set_cursor(0);
	itr.remove_current();
}

template <class T> void CircularList<T>::remove(int index)
{
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	itr.remove_current();
}

template <class T> T& CircularList<T>::operator[](int index)
{
	Iterator itr = Iterator(this);
	itr.set_cursor(index);
	return itr.get_current();
}
