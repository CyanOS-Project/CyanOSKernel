#include "DeviceManager.h"
#include "utils/ErrorCodes.h"

List<DeviceNode>* DeviceManager::m_devices;

void DeviceManager::setup()
{
	m_devices = new List<DeviceNode>;
}

Result<void> DeviceManager::add_device(const DeviceNode& node)
{
	if (m_devices->contains([&](DeviceNode& device) {
		    if (device.m_name == node.m_name)
			    return true;
		    return false;
	    })) {
		return ResultError(ERROR_DEVICE_ALREADY_EXISTS);
	}
	m_devices->push_back(node);
	return ResultError(ERROR_SUCCESS);
}

Result<void> DeviceManager::remove_device(const StringView& device_name)
{
	if (!m_devices->remove_if([&](DeviceNode& device) {
		    if (device.m_name == device_name)
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
		if (i.m_name == name) {
			return i;
		}
	}
	return ResultError(ERROR_DEVICE_DOESNOT_EXIST);
}