#include "PCIDevice.h"
#include "Arch/x86/Asm.h"

PCIDevice::PCIDevice(uint8_t bus_number, uint8_t slot_number, uint8_t function_number) :
    m_bus_number{bus_number},
    m_slot_number{slot_number},
    m_function_number{function_number}
{
}

uint8_t PCIDevice::bus(){
	return m_bus_number;
}

uint8_t PCIDevice::slot(){
	return m_slot_number;
}

uint8_t PCIDevice::function(){
	return m_function_number;
}

bool PCIDevice::does_exist()
{
	return vendor_id() != DEVICE_DOES_NOT_EXIST;
}

uint16_t PCIDevice::vendor_id()
{
	return read_config_word16(PCI_VENDOR_ID);
}

uint16_t PCIDevice::device_id()
{
	return read_config_word16(PCI_DEVICE_ID);
}

uint16_t PCIDevice::command()
{
	return read_config_word16(PCI_COMMAND);
}

uint16_t PCIDevice::status()
{
	return read_config_word16(PCI_STATUS);
}

uint8_t PCIDevice::revision_id()
{
	return read_config_word8(PCI_REVISION_ID);
}

uint8_t PCIDevice::prog_if()
{
	return read_config_word8(PCI_PROG_IF);
}

uint8_t PCIDevice::device_class()
{
	return read_config_word8(PCI_CLASS);
}

uint8_t PCIDevice::device_subclass()
{
	return read_config_word8(PCI_SUBCLASS);
}

uint8_t PCIDevice::cache_line_size()
{
	return read_config_word8(PCI_CACHE_LINE_SIZE);
}

uint8_t PCIDevice::latency_timer()
{
	return read_config_word8(PCI_LATENCY_TIMER);
}

PCIDevice::HeaderType PCIDevice::header_type()
{
	uint8_t header_type = read_config_word8(PCI_HEADER_TYPE) & 0x7F;
	return static_cast<HeaderType>(header_type);
}

bool PCIDevice::has_multiple_functions()
{
	return read_config_word8(PCI_HEADER_TYPE) & 0x80;
}

uint8_t PCIDevice::bist()
{
	return read_config_word8(PCI_BIST);
}

void PCIDevice::select_device(uint8_t offset)
{
	uint32_t address =
	    (m_bus_number << 16) | (m_slot_number << 11) | (m_function_number << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_ADDRESS_PORT, address);
}

uint8_t PCIDevice::read_config_word8(uint8_t offset)
{
	select_device(offset);
	return in8(PCI_VALUE_PORT + (offset & 0x3));
}

uint16_t PCIDevice::read_config_word16(uint8_t offset)
{
	select_device(offset);
	return in16(PCI_VALUE_PORT + (offset & 0x2));
}

uint32_t PCIDevice::read_config_word32(uint8_t offset)
{
	select_device(offset);
	return in32(PCI_VALUE_PORT);
}

void PCIDevice::write_config_word8(uint8_t offset, uint8_t data)
{
	select_device(offset);
	out8(PCI_VALUE_PORT + (offset & 0x3), data);
}

void PCIDevice::write_config_word16(uint8_t offset, uint16_t data)
{
	select_device(offset);
	out16(PCI_VALUE_PORT + (offset & 0x2), data);
}

void PCIDevice::write_config_word32(uint8_t offset, uint32_t data)
{
	select_device(offset);
	out32(PCI_VALUE_PORT, data);
}

PCIBridge::PCIBridge(uint8_t bus_number, uint8_t slot_number, uint8_t function_number) :
    PCIDevice{bus_number, slot_number, function_number}
{
}

uint32_t PCIBridge::BAR0()
{
	return read_config_word32(PCI_BAR0);
}

uint32_t PCIBridge::BAR1()
{
	return read_config_word32(PCI_BAR1);
}

uint8_t PCIBridge::primary_bus_number()
{
	return read_config_word32(PCI_PRIMARY_BUS_NUMBER);
}

uint8_t PCIBridge::secondary_bus_number()
{
	return read_config_word32(PCI_SECONDARY_BUS_NUMBER);
}

uint8_t PCIBridge::subordinate_bus_number()
{
	return read_config_word32(PCI_SUBORDINATE_BUS_NUMBER);
}

uint8_t PCIBridge::secondary_latency_timer()
{
	return read_config_word32(PCI_SECONDARY_LATENCY_TIMER);
}