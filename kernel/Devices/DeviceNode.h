#pragma once

#include "utils/Result.h"
#include "utils/String.h"
#include "utils/StringView.h"
#include "utils/types.h"

class DeviceManager;

class DeviceNode
{

  public:
	enum class Type { Keyboard, Mouse, Disk, Screen, Printer, USB, Other };

	virtual Result<void> receive(void* buffer, size_t count) = 0;
	virtual Result<void> send(void* buffer, size_t count) = 0;
	virtual Result<void> can_read() = 0;
	virtual Result<void> can_write() = 0;
	virtual Result<void> control() = 0;
	explicit DeviceNode(StringView device_name, Type device_type) : m_name{device_name}, m_type{device_type}
	{
	}
	~DeviceNode() = default;

  private:
	String m_name;
	Type m_type;

	friend class DeviceManager;
};
