#pragma once

#include "Filesystem/FileDescription.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/SpinLock.h"
#include "utils/List.h"
#include "utils/UniquePointer.h"
#include "utils/stl.h"
#include "utils/types.h"

template <class T> class Descriptor
{
  private:
	Spinlock m_lock;
	List<UniquePointer<T>> m_file_description_table; // FIXME:should be vector.

  public:
	Descriptor();
	~Descriptor();
	unsigned add_descriptor(UniquePointer<T>&& file_description);
	void remove_descriptor(unsigned);
	T& get_description(unsigned);
};

template <class T> unsigned Descriptor<T>::add_descriptor(UniquePointer<T>&& file_description)
{
	ScopedLock local_lock(m_lock);
	m_file_description_table.emplace_back(move(file_description));
	return m_file_description_table.size() - 1;
}

template <class T> void Descriptor<T>::remove_descriptor(unsigned file_descriptor)
{
	UNUSED(file_descriptor);
	ScopedLock local_lock(m_lock);
	// file_description_table.remove(file_descriptor);//FIXME:you can't just remove them.
}
template <class T> T& Descriptor<T>::get_description(unsigned file_descriptor)
{
	return *m_file_description_table[file_descriptor];
}
template <class T> Descriptor<T>::Descriptor() : m_lock{}, m_file_description_table{}
{
	m_lock.init();
}
template <class T> Descriptor<T>::~Descriptor()
{
}