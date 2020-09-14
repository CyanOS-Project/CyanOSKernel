#include "HandlesManager.h"
#include "Tasking/ScopedLock.h"

HandlesManager::HandlesManager() : m_lock{}, handle_table{} {}
HandlesManager::~HandlesManager() {}

Handle HandlesManager::add_handle(UniquePointer<FileDescription>&& file_description)
{
	ScopedLock local_lock(m_lock);

	auto& new_fd = file_description_table.push_back(move(file_description));
	handle_table.emplace_back(HandleType::FileDescription, new_fd.ptr());
	return handle_table.size() - 1;
}

Handle HandlesManager::add_handle(UniquePointer<ProcessDescription>&& process_description)
{
	ScopedLock local_lock(m_lock);

	auto& new_fp = process_description_table.push_back(move(process_description));
	handle_table.emplace_back(HandleType::ProcessDescription, new_fp.ptr());
	return handle_table.size() - 1;
}

void HandlesManager::remove_handle(Handle handle)
{
	ScopedLock local_lock(m_lock);

	auto& handle_entry = handle_table[handle];
	ASSERT(handle_entry.file_description);
	ASSERT(handle_entry.type != HandleType::Invalid);

	switch (handle_entry.type) {
		case HandleType::FileDescription:
			file_description_table.remove_if([&](UniquePointer<FileDescription>& fd) {
				if (fd.ptr() == handle_entry.file_description)
					return true;
				return false;
			});
			break;
		case HandleType::ProcessDescription:
			process_description_table.remove_if([&](UniquePointer<ProcessDescription>& fd) {
				if (fd.ptr() == handle_entry.process_description)
					return true;
				return false;
			});
			break;
		case HandleType::Invalid:
			ASSERT_NOT_REACHABLE();
			break;
	}
	handle_entry.type = HandleType::Invalid;
}

FileDescription& HandlesManager::get_file_description(Handle handle)
{
	ScopedLock local_lock(m_lock);

	auto& handle_entry = handle_table[handle];
	ASSERT(handle_entry.file_description);
	ASSERT(handle_entry.type == HandleType::FileDescription);

	return *handle_entry.file_description;
}

ProcessDescription& HandlesManager::get_process_description(Handle handle)
{
	ScopedLock local_lock(m_lock);

	auto& handle_entry = handle_table[handle];
	ASSERT(handle_entry.process_description);
	ASSERT(handle_entry.type == HandleType::ProcessDescription);

	return *handle_entry.process_description;
}

HandleType HandlesManager::check_handle(Handle handle)
{
	ScopedLock local_lock(m_lock);

	if (handle >= handle_table.size())
		return HandleType::Invalid;
	return handle_table[handle].type;
}