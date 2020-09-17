#pragma once

#include <Algorithms.h>
#include <Rule5.h>
#include <Types.h>

#ifdef __UNIT_TESTS
	#include <assert.h>
	#define ASSERT(x) assert(x)
#else
	#include <Assert.h>
#endif

template <class T> class List
{
  private:
	struct Node {
		T data;
		Node *next, *prev;
	};
	Node* m_head;
	Node* m_tail;
	size_t m_count;
	void remove_node(Node& new_node);
	void append_node(Node& new_node, Node* node);
	void prepend_node(Node& new_node, Node* node);
	class Iterator
	{
	  private:
		Node* m_current;

	  public:
		Iterator(Node* t_head, size_t index);
		explicit Iterator(Node* t_list);
		Iterator(const Iterator& other);
		void operator=(const List<T>::Iterator& other);
		~Iterator() = default;

		Iterator operator++(int);
		Iterator& operator++();
		bool operator!=(const List<T>::Iterator& other) const;
		bool operator==(const List<T>::Iterator& other) const;
		T* operator->();
		T& operator*();
		friend List<T>;
	};

  public:
	NON_COPYABLE(List);
	NON_MOVABLE(List);
	List();
	~List();

	Iterator begin();
	Iterator end();
	Iterator find(T& element);                                        // TODO: do unit test
	template <class Predicate> Iterator find_if(Predicate predicate); // TODO: do unit test

	template <typename... U> T& emplace_back(U&&... u);
	template <typename... U> T& emplace_front(U&&... u);
	T& push_back(const T& new_data);
	T& push_front(const T& new_data);
	T& push_back(T&& new_data);
	T& push_front(T&& new_data);
	void pop_back();
	void pop_front();
	void insert(const Iterator& node, const T& new_node);
	void remove(const Iterator&);
	template <class Predicate> bool remove_if(Predicate predicate);
	template <class Predicate> bool contains(Predicate predicate);
	void clear();
	void splice(List<T>& list, const Iterator& itr);
	bool is_empty() const;
	size_t size() const;
	T& head() const;
	T& tail() const;
	T& operator[](size_t index) const;
};

template <class T> List<T>::Iterator::Iterator(Node* t_head, size_t index) : m_current{t_head}
{
	while (index--) {
		m_current = m_current->next;
	}
}

template <class T> List<T>::Iterator::Iterator(const Iterator& other) : m_current{other.m_current} {}

template <class T> List<T>::Iterator::Iterator(Node* t_node) : m_current{t_node} {}

template <class T> typename List<T>::Iterator List<T>::Iterator::operator++(int arg)
{
	UNUSED(arg);
	Iterator old{*this};
	m_current = m_current->next;
	return old;
}

template <class T> typename List<T>::Iterator& List<T>::Iterator::operator++()
{
	m_current = m_current->next;
	return *this;
}

template <class T> bool List<T>::Iterator::operator!=(const List<T>::Iterator& other) const
{
	return m_current != other.m_current;
}

template <class T> bool List<T>::Iterator::operator==(const List<T>::Iterator& other) const
{
	return m_current == other.m_current;
}

template <class T> void List<T>::Iterator::operator=(const List<T>::Iterator& other)
{
	m_current = other->m_current;
}

template <class T> T& List<T>::Iterator::operator*()
{
	return m_current->data;
}

template <class T> T* List<T>::Iterator::operator->()
{
	return &m_current->data;
}

template <class T> List<T>::List() : m_head{nullptr}, m_tail{nullptr}, m_count{0} {}

template <class T> List<T>::~List()
{
	clear();
}

template <class T> void List<T>::clear()
{
	Node* node_iterator = m_head;
	while (node_iterator) {
		Node* next = node_iterator->next;
		delete node_iterator;
		node_iterator = next;
	}
	m_head = nullptr;
	m_count = 0;
}

template <class T> void List<T>::splice(List<T>& list, const Iterator& itr)
{
	remove_node(*itr.m_current);
	list.append_node(*itr.m_current, list.m_tail);
}

template <class T> void List<T>::remove_node(Node& node)
{
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
	m_count--;
}

template <class T> void List<T>::append_node(Node& new_node, Node* node)
{
	if (!m_head) {
		new_node.prev = nullptr;
		new_node.next = nullptr;
		m_head = m_tail = &new_node;
	} else {
		new_node.prev = node;
		new_node.next = node->next;
		if (node->next) {
			node->next->prev = &new_node;
		} else {
			m_tail = &new_node;
		}
		node->next = &new_node;
	}
	m_count++;
}

template <class T> void List<T>::prepend_node(Node& new_node, Node* node)
{
	if (!m_head) {
		new_node.prev = nullptr;
		new_node.next = nullptr;
		m_head = m_tail = &new_node;
	} else {
		new_node.prev = node->prev;
		new_node.next = node;
		if (node->prev) {
			node->prev->next = &new_node;
		} else {
			m_head = &new_node;
		}
		node->prev = &new_node;
	}
	m_count++;
}

template <class T> typename List<T>::Iterator List<T>::begin()
{
	return Iterator(m_head);
}

template <class T> typename List<T>::Iterator List<T>::end()
{
	return Iterator(nullptr);
}

template <class T> typename List<T>::Iterator List<T>::find(T& element)
{
	for (auto i = begin(); i != end(); i++) {
		if (&element == &*i) {
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

template <class T> template <typename... U> T& List<T>::emplace_back(U&&... u)
{
	Node* new_node = new Node{T{forward<U>(u)...}, nullptr, nullptr}; // FIXME: remove T{}
	append_node(*new_node, m_tail);
	return new_node->data;
}

template <class T> template <typename... U> T& List<T>::emplace_front(U&&... u)
{
	Node* new_node = new Node{T{forward<U>(u)...}, nullptr, nullptr};
	prepend_node(*new_node, m_head);
	return new_node->data;
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

template <class T> template <class Predicate> bool List<T>::contains(Predicate predicate)
{
	for (auto&& i : *this) {
		if (predicate(i)) {
			return true;
		}
	}
	return false;
}

template <class T> T& List<T>::push_back(const T& new_data)
{
	Node* new_node = new Node{new_data, nullptr, nullptr};
	append_node(*new_node, m_tail);
	return new_node->data;
}

template <class T> T& List<T>::push_front(const T& new_data)
{
	Node* new_node = new Node{new_data, nullptr, nullptr};
	prepend_node(*new_node, m_head);
	return new_node->data;
}

template <class T> T& List<T>::push_back(T&& new_data)
{
	Node* new_node = new Node{move(new_data), nullptr, nullptr};
	append_node(*new_node, m_tail);
	return new_node->data;
}

template <class T> T& List<T>::push_front(T&& new_data)
{
	Node* new_node = new Node{move(new_data), nullptr, nullptr};
	prepend_node(*new_node, m_head);
	return new_node->data;
}

template <class T> void List<T>::pop_back()
{
	ASSERT(m_tail);
	remove_node(*m_tail);
	delete m_tail;
}

template <class T> void List<T>::pop_front()
{
	ASSERT(m_head);
	remove_node(*m_head);
	delete m_head;
}

template <class T> void List<T>::insert(const Iterator& pos, const T& new_data)
{
	Node* new_node = new Node{new_data, nullptr, nullptr};
	append_node(*new_node, pos.m_current);
}

template <class T> void List<T>::remove(const Iterator& itr)
{
	ASSERT(itr.m_current);
	remove_node(*itr.m_current);
	delete itr.m_current;
}

template <class T> T& List<T>::head() const
{
	ASSERT(m_head);
	return m_head->data;
}

template <class T> T& List<T>::tail() const
{
	ASSERT(m_head);
	return m_tail->data;
}

template <class T> T& List<T>::operator[](size_t index) const
{
	ASSERT(index < m_count);
	Iterator itr(m_head, index);
	return *itr;
}

template <class T> bool List<T>::is_empty() const
{
	return !size();
}

template <class T> size_t List<T>::size() const
{
	return m_count;
}