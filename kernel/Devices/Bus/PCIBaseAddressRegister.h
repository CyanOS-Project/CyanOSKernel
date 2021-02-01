#pragma once
#include <Types.h>

class PCIBaseAddressRegister
{

  public:
	enum class Type { MemoryMapped, IOMapped };
	enum class Size { BAR32, BAR64, None };
	PCIBaseAddressRegister(uint32_t value);
	Type type();
	Size size();
	uint32_t address();

  private:
	uint32_t m_value;
	Type m_type;
};