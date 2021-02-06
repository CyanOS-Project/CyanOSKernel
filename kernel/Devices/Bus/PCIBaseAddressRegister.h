#pragma once
#include <Types.h>

class PCIBaseAddressRegister
{

  public:
	enum class Type { MemoryMapped, IOMapped };
	enum class Size { BAR32, BAR64, None };
	PCIBaseAddressRegister(uint32_t value);
	Type type() const;
	Size size() const;
	uint16_t io_address() const;
	uint32_t mmio_address() const;

  private:
	uint32_t m_value;
	Type m_type;
};