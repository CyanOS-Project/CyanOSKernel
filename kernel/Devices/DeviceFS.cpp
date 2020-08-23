#include "DeviceFS.h"
#include "Tasking/ScopedLock.h"
#include "utils/ErrorCodes.h"
#include "utils/stl.h"

List<UniquePointer<FSNode>>* DeviceFS::children;

void DeviceFS::init()
{
	children = new List<UniquePointer<FSNode>>;
}

DeviceFS& DeviceFS::alloc()
{
	return *new DeviceFS;
}

DeviceFS::DeviceFS() : FSNode{"Devices", 0, 0, NodeType::Root, 0}
{
}

DeviceFS::~DeviceFS()
{
}

Result<FSNode&> DeviceFS::create(const StringView& name, OpenMode mode, OpenFlags flags)
{
	UNUSED(name);
	UNUSED(mode);
	UNUSED(flags);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceFS::open(OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);

	return ResultError(ERROR_SUCCESS);
}

Result<void> DeviceFS::close()
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> DeviceFS::read(void* buff, size_t offset, size_t size)
{
	UNUSED(buff);
	UNUSED(offset);
	UNUSED(size);

	return ResultError(ERROR_SUCCESS);
}

Result<void> DeviceFS::write(const void* buff, size_t offset, size_t size)
{
	UNUSED(buff);
	UNUSED(offset);
	UNUSED(size);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<bool> DeviceFS::can_read()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<bool> DeviceFS::can_write()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceFS::remove()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceFS::mkdir(const StringView& name, int flags, int access)
{
	UNUSED(name);
	UNUSED(flags);
	UNUSED(access);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceFS::link(FSNode& node)
{
	UNUSED(node);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceFS::unlink(FSNode& node)
{
	UNUSED(node);
	return ResultError(ERROR_INVALID_OPERATION);
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