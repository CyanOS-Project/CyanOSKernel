#include "pcibase_address_register.h"

PCIBaseAddressRegister::PCIBaseAddressRegister(u32 value) : m_value{value}
{
	if (m_value & 1) {
		m_type = Type::IOMapped;
	} else {
		m_type = Type::MemoryMapped;
	}
}

PCIBaseAddressRegister::Type PCIBaseAddressRegister::type() const
{
	return m_type;
}

PCIBaseAddressRegister::Size PCIBaseAddressRegister::size() const
{
	if (m_type == Type::MemoryMapped) {
		uint8_t size = (m_value & 0x6) >> 1;
		if (size == 0x0) {
			return Size::BAR32;
		} else if (size == 0x2) {
			return Size::BAR64;
		} else {
			return Size::None;
		}
	} else {
		return Size::None;
	}
}

u16 PCIBaseAddressRegister::io_address() const
{
	return m_value & 0xFFFFFFFC;
}

u32 PCIBaseAddressRegister::mmio_address() const
{
	return m_value & 0xFFFFFFF0;
}