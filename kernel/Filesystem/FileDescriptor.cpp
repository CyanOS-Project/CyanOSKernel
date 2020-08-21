#include "FileDescriptor.h"
#include "Filesystem/FileDescription.h"
#include "Tasking/ScopedLock.h"

unsigned FileDescriptor::add_descriptor(const FileDescription& file_description)
{
	ScopedLock local_lock(m_lock);
	m_file_description_table.emplace_back(file_description);
	return m_file_description_table.size() - 1;
}

void FileDescriptor::remove_descriptor(unsigned file_descriptor)
{
	ScopedLock local_lock(m_lock);
	// file_description_table.remove(file_descriptor);
}

FileDescription& FileDescriptor::get_description(unsigned file_descriptor)
{
	return m_file_description_table[file_descriptor];
}

FileDescriptor::FileDescriptor() : m_lock{}, m_file_description_table{}
{
	m_lock.init();
}

FileDescriptor::~FileDescriptor()
{
}