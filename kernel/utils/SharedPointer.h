#pragma once

#include "assert.h"
#include "stl.h"
#include "types.h"

template <typename T> class SharedPointer
{
  public:
	template <class... Args> static SharedPointer<T> make_shared(Args&&... args)
	{
		return SharedPointer(new T{forward<Args>(args)...});
	}

	explicit SharedPointer(T* ptr) : m_storage{ptr}, m_reference_count{new size_t{0}}
	{
		assert(m_storage);
	}

	SharedPointer(SharedPointer&& other) : m_storage{other.m_storage}, m_reference_count{other.m_reference_count}
	{
		other.m_storage = nullptr;
		other.m_reference_count = nullptr;
	}

	SharedPointer& operator=(SharedPointer&& other)
	{
		ASSERT(this != &other);
		check_and_delete_object();
		m_storage = other.m_storage;
		m_reference_count = other.m_reference_count;
		other.m_storage = nullptr;
		other.m_reference_count = nullptr;
		return *this;
	}

	SharedPointer(const SharedPointer& other) : m_storage{other.m_storage}, m_reference_count{other.m_reference_count}
	{
		(*m_reference_count)++;
	}

	SharedPointer& operator=(const SharedPointer& other)
	{
		ASSERT(this != &other);
		check_and_delete_object();
		m_storage = other.m_storage;
		m_reference_count = other.m_reference_count;
		(*m_reference_count)++;
		return *this;
	}

	~SharedPointer()
	{
		check_and_delete_object();
	}

	T& operator*()
	{
		return *m_storage;
	}

	T* operator->()
	{
		return m_storage;
	}

	bool operator==(const SharedPointer& other)
	{
		return (m_storage == other.m_storage);
	}

	bool operator!=(const SharedPointer& other)
	{
		return (m_storage != other.m_storage);
	}

	T* get()
	{
		return m_storage;
	}

	size_t use_count()
	{
		return *m_reference_count;
	}

  private:
	T* m_storage = nullptr;
	size_t* m_reference_count = nullptr;
	void check_and_delete_object()
	{
		size_t tmp_ref_count = *m_reference_count;
		if (tmp_ref_count == 0) {
			delete m_storage;
			delete m_reference_count;
			m_storage = nullptr;
			m_reference_count = nullptr;
		} else {
			(*m_reference_count)--;
			m_storage = nullptr;
		}
	}
};