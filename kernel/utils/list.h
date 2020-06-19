
template <class T> struct Node {
	T data;
	Node<T>*next, *prev;
};

template <class T> class CircularList
{
  private:
	Node<T>* m_first;
	Node<T>* m_last;

  public:
	unsigned count;

	CircularList()
	{
		m_first = nullptr;
		m_last = nullptr;
		count = 0;
	}

	void push_back(T new_data)
	{
		Node<T>* new_node = new Node<T>;
		if (m_first) {
			new_node->next = m_first;
			new_node->prev = m_first->prev;
			m_first->prev->next = new_node;
			m_first->next->prev = new_node;
			new_node->data = new_data;
		} else {
			new_node->data = new_data;
			m_first = new_node->next = new_node->prev = new_node;
		}
		count++;
	}

	void push_front(T new_data)
	{
	}

	void remove_node(Node<T>* node)
	{
		node->prev->next = node->next;
		node->prev->next = node->prev;
		count--;
	}

	Node<T>* begin()
	{
		return m_first;
	}

	Node<T>* end()
	{
		/*CircularList<T>* node_iterator = begin();
		do {
		    ++node_iterator;
		} while (node_iterator != begin());
		return node_iterator;*/
	}

	T& operator[](int index)
	{
		Node<T>* p = m_first;
		while (index--) {
			p = p->next;
		}
		return p->data;
	}
};
