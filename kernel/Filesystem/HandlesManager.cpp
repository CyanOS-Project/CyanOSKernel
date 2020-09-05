#include "HandlesManager.h"

HandlesManager::HandlesManager() : m_lock{}, handle_table{} {}
HandlesManager::~HandlesManager() {}

Handle HandlesManager::add_file_description(UniquePointer<FileDescription>&& file_description)
{
	ScopedLock local_lock(m_lock);
	handle_table.emplace_back(true, move(file_description));
	return handle_table.size() - 1;
}

void HandlesManager::remove_handle(Handle handle)
{
	ASSERT(check_handle(handle));

	ScopedLock local_lock(m_lock);
	handle_table[handle].valid = false;
	handle_table[handle].file_description.~UniquePointer();
}

FileDescription& HandlesManager::get_file_description(Handle handle)
{
	ASSERT(check_handle(handle));

	ScopedLock local_lock(m_lock);
	auto& handle_entry = handle_table[handle];
	return *handle_entry.file_description;
}

bool HandlesManager::check_handle(Handle handle)
{
	if (handle >= handle_table.size())
		return false;
	return handle_table[handle].valid;
}