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
	bool m_is_open = true;

  public:
	Result<void> receive(void* buffer, size_t count);
	Result<void> send(void* buffer, size_t count);
	Result<void> can_read();
	Result<void> can_write();
	Result<void> control();
	Result<void> close();
	~DeviceDescription();

	friend class DeviceManager;
};
