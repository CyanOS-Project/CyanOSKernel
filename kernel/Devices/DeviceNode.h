#include "utils/Result.h"
#include "utils/String.h"
#include "utils/StringView.h"
#include "utils/types.h"

class DeviceManager;

class DeviceNode
{

  public:
	enum class Type { Keyboard, Mouse, Disk, Screen, Printer, USB, Other };

	Result<void> receive(void* buffer, size_t count);
	Result<void> send(void* buffer, size_t count);
	Result<void> can_read();
	Result<void> can_write();
	Result<void> control();
	explicit DeviceNode(StringView device_name, Type device_type) : m_name{device_name}, m_type{device_type}
	{
	}
	~DeviceNode() = default;

  private:
	String m_name;
	Type m_type;

	friend class DeviceManager;
};
