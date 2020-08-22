#include "FileDescriptor.h"
#include "Filesystem/FileDescription.h"
#include "Tasking/ScopedLock.h"
#include "utils/stl.h"

unsigned FileDescriptor::add_descriptor(UniquePointer<FileDescription>&& file_description)
{
	ScopedLock local_lock(m_lock);
	m_file_description_table.emplace_back(move(file_description));
	return m_file_description_table.size() - 1;
}

void FileDescriptor::remove_descriptor(unsigned file_descriptor)
{
	UNUSED(file_descriptor);
	ScopedLock local_lock(m_lock);
	// file_description_table.remove(file_descriptor);//FIXME:you can't just remove them.
}

FileDescription& FileDescriptor::get_description(unsigned file_descriptor)
{
	return *m_file_description_table[file_descriptor];
}

FileDescriptor::FileDescriptor() : m_lock{}, m_file_description_table{}
{
	m_lock.init();
}

FileDescriptor::~FileDescriptor()
{
}