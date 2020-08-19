#include "DeviceDescription.h"
#include "utils/ErrorCodes.h"

DeviceDescription::DeviceDescription(DeviceNode& device) : m_device{device}
{
}

Result<void> DeviceDescription::receive(void* buffer, size_t count)
{
	ASSERT(m_is_open);

	return m_device.receive(buffer, count);
}

Result<void> DeviceDescription::send(void* buffer, size_t count)
{
	ASSERT(m_is_open);

	return m_device.send(buffer, count);
}

Result<void> DeviceDescription::can_read()
{
	ASSERT(m_is_open);

	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceDescription::can_write()
{
	ASSERT(m_is_open);

	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceDescription::control()
{
	ASSERT(m_is_open);

	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> DeviceDescription::close()
{
	ASSERT(m_is_open);

	m_is_open = false;
	return m_device.close();
}

DeviceDescription::~DeviceDescription()
{
	if (m_is_open) {
		close();
	}
}