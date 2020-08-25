#include "DeviceFS.h"
#include "Tasking/ScopedLock.h"
#include "Utils/ErrorCodes.h"
#include "Utils/Stl.h"

List<UniquePointer<FSNode>>* DeviceFS::children = nullptr;

UniquePointer<FSNode> DeviceFS::alloc(const StringView& name)
{
	return UniquePointer<FSNode>(new DeviceFS(name));
}

DeviceFS::DeviceFS(const StringView& name) : FSNode{name, 0, 0, NodeType::Root, 0}
{
	ASSERT(children == 0);
	children = new List<UniquePointer<FSNode>>;
}

DeviceFS::~DeviceFS()
{
}

Result<FSNode&> DeviceFS::dir_lookup(const StringView& file_name)
{
	for (auto& i : *children) {
		if (i->m_name == file_name) {
			return *i;
		}
	}
	return ResultError(ERROR_DEVICE_DOESNOT_EXIST);
}

Result<void> DeviceFS::add_device(UniquePointer<FSNode>&& new_device)
{
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
	return ResultError(ERROR_INVALID_OPERATION);
}