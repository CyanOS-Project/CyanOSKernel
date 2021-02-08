#pragma once
#include <Types.h>

class PCIBaseAddressRegister
{

  public:
	enum class Type
	{
		MemoryMapped,
		IOMapped
	};
	enum class Size
	{
		BAR32,
		BAR64,
		None
	};
	PCIBaseAddressRegister(u32 value);
	Type type() const;
	Size size() const;
	u16 io_address() const;
	u32 mmio_address() const;

  private:
	u32 m_value;
	Type m_type;
};