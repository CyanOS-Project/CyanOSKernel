#pragma once
#include "DeviceNode.h"
#include "utils/Result.h"
#include "utils/types.h"

class DeviceManager;

class DeviceDescription
{
  private:
	DeviceNode& m_device;
	DeviceDescription(DeviceNode& device);

  public:
	Result<void> receive(void* buffer, size_t count);
	Result<void> send(void* buffer, size_t count);
	Result<void> can_read();
	Result<void> can_write();
	Result<void> control();
	~DeviceDescription();

	friend class DeviceManager;
};
