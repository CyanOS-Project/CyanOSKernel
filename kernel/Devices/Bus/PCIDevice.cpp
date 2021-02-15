#include "PCIDevice.h"
#include "Arch/x86/Asm.h"

PCIDevice::PCIDevice(u8 bus_number, u8 slot_number, u8 function_number) :
    m_bus_number{bus_number},
    m_slot_number{slot_number},
    m_function_number{function_number}
{
}

u8 PCIDevice::bus() const
{
	return m_bus_number;
}

u8 PCIDevice::slot() const
{
	return m_slot_number;
}

u8 PCIDevice::function() const
{
	return m_function_number;
}

bool PCIDevice::does_exist() const
{
	return vendor_id() != DEVICE_DOES_NOT_EXIST;
}

u16 PCIDevice::vendor_id() const
{
	return read_config_word16(PCI_PORT_VENDOR_ID);
}

u16 PCIDevice::device_id() const
{
	return read_config_word16(PCI_PORT_DEVICE_ID);
}

u16 PCIDevice::command() const
{
	return read_config_word16(PCI_PORT_COMMAND);
}

u16 PCIDevice::status() const
{
	return read_config_word16(PCI_PORT_STATUS);
}

u8 PCIDevice::revision_id() const
{
	return read_config_word8(PCI_PORT_REVISION_ID);
}

u8 PCIDevice::prog_if() const
{
	return read_config_word8(PCI_PORT_PROG_IF);
}

u8 PCIDevice::device_class() const
{
	return read_config_word8(PCI_PORT_CLASS);
}

u8 PCIDevice::device_subclass() const
{
	return read_config_word8(PCI_PORT_SUBCLASS);
}

u8 PCIDevice::cache_line_size() const
{
	return read_config_word8(PCI_PORT_CACHE_LINE_SIZE);
}

u8 PCIDevice::latency_timer() const
{
	return read_config_word8(PCI_PORT_LATENCY_TIMER);
}

PCIDevice::HeaderType PCIDevice::header_type() const
{
	u8 header_type = read_config_word8(PCI_PORT_HEADER_TYPE) & 0x7F;
	return static_cast<HeaderType>(header_type);
}

bool PCIDevice::has_multiple_functions() const
{
	return read_config_word8(PCI_PORT_HEADER_TYPE) & 0x80;
}

u8 PCIDevice::bist() const
{
	return read_config_word8(PCI_PORT_BIST);
}

void PCIDevice::enable_bus_mastering()
{
	u16 command_port = read_config_word16(PCI_PORT_COMMAND);
	write_config_word16(PCI_PORT_COMMAND, command_port | PCI_COMMAND_BUS_MASTER);
}

void PCIDevice::disable_bus_mastering()
{
	u16 command_port = read_config_word16(PCI_PORT_COMMAND);
	write_config_word16(PCI_PORT_COMMAND, command_port & (~PCI_COMMAND_BUS_MASTER));
}

void PCIDevice::disable_interrupts()
{
	u16 command_port = read_config_word16(PCI_PORT_COMMAND);
	write_config_word16(PCI_PORT_COMMAND, command_port | PCI_COMMAND_INTERRUPT_DISABLE);
}

void PCIDevice::enable_interrupts()
{
	u16 command_port = read_config_word16(PCI_PORT_COMMAND);
	write_config_word16(PCI_PORT_COMMAND, command_port & (~PCI_COMMAND_INTERRUPT_DISABLE));
}

void PCIDevice::select_port(u8 offset) const
{
	u32 address = (m_bus_number << 16) | (m_slot_number << 11) | (m_function_number << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_PORT_ADDRESS_PORT, address);
}

u8 PCIDevice::read_config_word8(u8 offset) const
{
	select_port(offset);
	return in8(PCI_PORT_VALUE_PORT + (offset & 0x3));
}

u16 PCIDevice::read_config_word16(u8 offset) const
{
	select_port(offset);
	return in16(PCI_PORT_VALUE_PORT + (offset & 0x2));
}

u32 PCIDevice::read_config_word32(u8 offset) const
{
	select_port(offset);
	return in32(PCI_PORT_VALUE_PORT);
}

void PCIDevice::write_config_word8(u8 offset, u8 data)
{
	select_port(offset);
	out8(PCI_PORT_VALUE_PORT + (offset & 0x3), data);
}

void PCIDevice::write_config_word16(u8 offset, u16 data)
{
	select_port(offset);
	out16(PCI_PORT_VALUE_PORT + (offset & 0x2), data);
}

void PCIDevice::write_config_word32(u8 offset, u32 data)
{
	select_port(offset);
	out32(PCI_PORT_VALUE_PORT, data);
}

PCIBridge::PCIBridge(u8 bus_number, u8 slot_number, u8 function_number) :
    PCIDevice{bus_number, slot_number, function_number}
{
}

PCIBridge::PCIBridge(const PCIDevice& device) : PCIDevice{device} {}

PCIBaseAddressRegister PCIBridge::BAR0() const
{
	return read_config_word32(PCI_PORT_BAR0);
}

PCIBaseAddressRegister PCIBridge::BAR1() const
{
	return read_config_word32(PCI_PORT_BAR1);
}

u8 PCIBridge::primary_bus_number() const
{
	return read_config_word32(PCI_PORT_PRIMARY_BUS_NUMBER);
}

u8 PCIBridge::secondary_bus_number() const
{
	return read_config_word32(PCI_PORT_SECONDARY_BUS_NUMBER);
}

u8 PCIBridge::subordinate_bus_number() const
{
	return read_config_word32(PCI_PORT_SUBORDINATE_BUS_NUMBER);
}

u8 PCIBridge::secondary_latency_timer() const
{
	return read_config_word32(PCI_PORT_SECONDARY_LATENCY_TIMER);
}

GenericPCIDevice::GenericPCIDevice(const PCIDevice& device) : PCIDevice{device} {}

GenericPCIDevice::GenericPCIDevice(u8 bus_number, u8 slot_number, u8 function_number) :
    PCIDevice{bus_number, slot_number, function_number}
{
}

PCIBaseAddressRegister GenericPCIDevice::BAR0() const
{
	return read_config_word32(PCI_PORT_BAR0);
}

PCIBaseAddressRegister GenericPCIDevice::BAR1() const
{
	return read_config_word32(PCI_PORT_BAR1);
}

PCIBaseAddressRegister GenericPCIDevice::BAR2() const
{
	return read_config_word32(PCI_PORT_BAR2);
}

PCIBaseAddressRegister GenericPCIDevice::BAR3() const
{
	return read_config_word32(PCI_PORT_BAR3);
}

PCIBaseAddressRegister GenericPCIDevice::BAR4() const
{
	return read_config_word32(PCI_PORT_BAR4);
}

PCIBaseAddressRegister GenericPCIDevice::BAR5() const
{
	return read_config_word32(PCI_PORT_BAR5);
}

u8 GenericPCIDevice::interrupt_line() const
{
	return read_config_word8(PCI_PORT_INTERRUPT_LINE);
}

u8 GenericPCIDevice::interrupt_pin() const
{
	return read_config_word8(PCI_PORT_INTERRUPT_PIN);
}