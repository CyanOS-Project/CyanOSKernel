#include "DeviceFS.h"
#include "Tasking/ScopedLock.h"
#include "Utils/Algorithms.h"
#include "Utils/ErrorCodes.h"

List<UniquePointer<FSNode>>* DeviceFS::children = nullptr;

UniquePointer<FSNode> DeviceFS::alloc(const StringView& name)
{
	return UniquePointer<FSNode>(new DeviceFS(name));
}

DeviceFS::DeviceFS(const StringView& name) : FSNode{name, 0, 0, NodeType::Root, 0}
{
	ASSERT(children == 0);
	// ScopedLock local_lock(m_lock);
	children = new List<UniquePointer<FSNode>>;
}

DeviceFS::~DeviceFS() {}

Result<void> DeviceFS::add_device(UniquePointer<FSNode>&& new_device)
{
	// ScopedLock local_lock(m_lock);
	if (children->contains([&](UniquePointer<FSNode>& device) {
		    if (device->m_name == new_device->m_name)
			    return true;
		    return false;
	    })) {
		return ResultError(ERROR_DEVICE_ALREADY_EXISTS);
	}

	children->push_back(move(new_device));
	return ResultError(ERROR_SUCCESS);
}

Result<void> DeviceFS::remove_device(const StringView& name)
{
	UNUSED(name);
	// ScopedLock local_lock(m_lock);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<FSNode&> DeviceFS::dir_lookup(const StringView& file_name)
{
	// ScopedLock local_lock(m_lock);
	for (auto& i : *children) {
		if (i->m_name == file_name) {
			return *i;
		}
	}
	return ResultError(ERROR_DEVICE_DOESNOT_EXIST);
}

Result<StringView> DeviceFS::dir_query(size_t index)
{
	UNUSED(index);
	// ScopedLock local_lock(m_lock);
	/*if (index >= children->size())
	    return ResultError(ERROR_INVALID_PARAMETERS);

	// FIXME: it's not efficeint way to iterate them all again;
	auto itr = children->begin();
	while (index--) {
	    itr++;
	}
	return StringView(itr->m_name);*/
	return ResultError(ERROR_INVALID_OPERATION);
}