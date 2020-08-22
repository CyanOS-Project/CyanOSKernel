#include "DeviceManager.h"
#include "utils/ErrorCodes.h"
#include "utils/stl.h"

List<UniquePointer<DeviceNode>>* DeviceManager::m_devices;

Result<UniquePointer<DeviceDescription>> DeviceManager::open(const StringView& path, int mode, int flags)
{
	UNUSED(mode);
	UNUSED(flags);
	auto dev_ret = get_device(path);
	if (dev_ret.is_error()) {
		return ResultError(dev_ret.error());
	}

	auto open_ret = dev_ret.value().open(mode, flags);
	if (open_ret.is_error()) {
		return ResultError(open_ret.error());
	}

	return UniquePointer<DeviceDescription>::make_unique(dev_ret.value());
}

void DeviceManager::setup()
{
	m_devices = new List<UniquePointer<DeviceNode>>;
}

Result<void> DeviceManager::add_device(UniquePointer<DeviceNode>&& node)
{
	if (m_devices->contains([&](UniquePointer<DeviceNode>& device) {
		    if (device->m_name == node->m_name)
			    return true;
		    return false;
	    })) {
		return ResultError(ERROR_DEVICE_ALREADY_EXISTS);
	}
	m_devices->emplace_back(move(node));
	return ResultError(ERROR_SUCCESS);
}

Result<void> DeviceManager::remove_device(const StringView& device_name)
{
	if (!m_devices->remove_if([&](UniquePointer<DeviceNode>& device) {
		    if (device->m_name == device_name)
			    return true;
		    return false;
	    })) {
		return ResultError(ERROR_DEVICE_DOESNOT_EXIST);
	}
	return ResultError(ERROR_SUCCESS);
}

Result<DeviceNode&> DeviceManager::get_device(const StringView& name)
{
	for (auto&& i : *m_devices) {
		if (i->m_name == name) {
			return *i;
		}
	}
	return ResultError(ERROR_DEVICE_DOESNOT_EXIST);
}