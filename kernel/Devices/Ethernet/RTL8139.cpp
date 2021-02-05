#include "RTL8139.h"
#include "Arch/x86/Asm.h"
#include "Arch/x86/Isr.h"
#include "Arch/x86/Pic.h"
#include "Devices/DebugPort/Logger.h"
#include "Tasking/Thread.h"
#include "VirtualMemory/Memory.h"

static void irq_handler(ISRContextFrame& context);
RTL8139* instance = nullptr;

void RTL8139::turn_on()
{
	write_register8(RTL8139_PORT_CONFIG1, 0);
}

void RTL8139::software_rest()
{
	write_register8(RTL8139_PORT_CHIP_CMD, RTL8139_COMMAND_RESET);
	while (read_register8(RTL8139_PORT_CHIP_CMD) & RTL8139_COMMAND_RESET) {
	}
}

void RTL8139::setup_TX_buffers()
{
	for (size_t i = 0; i < NUMBER_TX_BUFFERS; i++) {
		m_tx_buffers[i] = valloc(0, MAX_TX_BUFFER_SIZE, PAGE_READWRITE | PAGE_CONTAGIOUS);
	}
}

void RTL8139::configure_rx()
{
	write_register32(RTL8139_PORT_RX_CONFIG,
	                 RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS | RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS |
	                     RTL8139_RX_CONFIG_ACCEPT_MULTICAST_PACKETS | RTL8139_RX_CONFIG_ACCEPT_BROADCAST_PACKETS |
	                     RTL8139_RX_CONFIG_EEPROM_WRAP);
}

void RTL8139::configure_RX_TX()
{
	write_register16(RTL8139_PORT_INTR_MASK, RTL8139_INTERRUPT_MASK_RX_OK | RTL8139_INTERRUPT_MASK_TX_OK);

	write_register32(RTL8139_PORT_RX_CONFIG, RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS |
	                                             RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_ADDRESS_PACKETS |
	                                             RTL8139_RX_CONFIG_ACCEPT_MULTICAST_PACKETS |
	                                             RTL8139_RX_CONFIG_ACCEPT_BROADCAST_PACKETS |
	                                             RTL8139_RX_CONFIG_EEPROM_WRAP);

	write_register8(RTL8139_PORT_CHIP_CMD, RTL8139_COMMAND_RX_ENABLE | RTL8139_COMMAND_TX_ENABLE);
}

void RTL8139::rx_tx_handler()
{
	info() << "Descriptors: " << Hex(read_register16(RTL8139_PORT_TX_SUMMARY));

	uint16_t status = read_register16(RTL8139_PORT_INTR_STATUS);
	info() << "ISR status: " << status;
	if (status & RTL8139_INTERRUPT_STATUS_TX_OK) {
		info() << "Interrupt: Data has been sent!";
	}
	if (status & RTL8139_INTERRUPT_STATUS_RX_OK) {
		info() << "Interrupt: Data has been received!";
	}

	write_register16(RTL8139_PORT_INTR_STATUS, status);
}

void RTL8139::send_packet(const void* data, size_t len)
{
	if (len > MAX_TX_BUFFER_SIZE) {
		warn() << "Data too large to be sent!";
		return;
	}

	memcpy(m_tx_buffers[m_current_TX_buffer], data, len);
	write_register32(TSAD_array[m_current_TX_buffer], virtual_to_physical_address(m_tx_buffers[m_current_TX_buffer]));
	write_register32(TSD_array[m_current_TX_buffer], len);
	while (!(read_register32(TSD_array[m_current_TX_buffer]) & RTL8139_TX_STATUS_TOK)) {
	}
	m_current_TX_buffer = (m_current_TX_buffer + 1) % NUMBER_TX_BUFFERS;
}

RTL8139::RTL8139(GenericPCIDevice&& device)
{
	instance = this;
	m_ports = device.BAR0().address();

	info() << "Device MAC address: " << Hex(read_register8(0)) << " " << Hex(read_register8(1)) << " "
	       << Hex(read_register8(2)) << " " << Hex(read_register8(3)) << " " << Hex(read_register8(4)) << " "
	       << Hex(read_register8(5)) << " ";

	device.enable_bus_mastering();
	device.enable_interrupts();

	turn_on();
	software_rest();
	configure_RX_TX();
	setup_TX_buffers();

	ISR::register_hardware_interrupt_handler(irq_handler, device.interrupt_line());

	send_packet("hello there.", 10);
	Thread::sleep(1000);
	send_packet("hello there...", 25);
}

void RTL8139::write_register8(uint16_t address, uint8_t value)
{
	out8(m_ports + address, value);
}

void RTL8139::write_register16(uint16_t address, uint16_t value)
{
	out16(m_ports + address, value);
}

void RTL8139::write_register32(uint16_t address, uint32_t value)
{
	out32(m_ports + address, value);
}

uint8_t RTL8139::read_register8(uint16_t address)
{
	return in8(m_ports + address);
}

uint16_t RTL8139::read_register16(uint16_t address)
{
	return in16(m_ports + address);
}

uint32_t RTL8139::read_register32(uint16_t address)
{
	return in32(m_ports + address);
}

static void irq_handler(ISRContextFrame& context)
{
	UNUSED(context);
	ASSERT(instance);
	instance->rx_tx_handler();
}