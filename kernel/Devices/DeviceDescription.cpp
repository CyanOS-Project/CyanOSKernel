#include "DeviceDescription.h"
#include "utils/ErrorCodes.h"

DeviceDescription::DeviceDescription(DeviceNode& device) : m_device{device}
{
}

Result<void> DeviceDescription::receive(void* buffer, size_t count)
{
	return m_device.receive(buffer, count);
}

Result<void> DeviceDescription::send(void* buffer, size_t count)
{
	return m_device.send(buffer, count);
}

Result<void> DeviceDescription::can_read()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceDescription::can_write()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceDescription::control()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

DeviceDescription::~DeviceDescription()
{
}