#include "PCIBaseAddressRegister.h"

PCIBaseAddressRegister::PCIBaseAddressRegister(uint32_t value) : m_value{value}
{
	if (m_value & 1) {
		m_type = Type::IOMapped;
	} else {
		m_type = Type::MemoryMapped;
	}
}

PCIBaseAddressRegister::Type PCIBaseAddressRegister::type()
{
	return m_type;
}

PCIBaseAddressRegister::Size PCIBaseAddressRegister::size()
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

uint32_t PCIBaseAddressRegister::address()
{
	if (m_type == Type::MemoryMapped) {
		return m_value & 0xFFFFFFF0;
	} else {
		return m_value & 0xFFFFFFFC;
	}
}