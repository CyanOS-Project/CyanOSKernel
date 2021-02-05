#include "RTL8139.h"
#include "Arch/x86/Asm.h"
#include "Arch/x86/Isr.h"
#include "Arch/x86/Pic.h"
#include "Devices/DebugPort/Logger.h"
#include "Tasking/Thread.h"
#include "VirtualMemory/Memory.h"

static uint16_t ports = 0;
static void* tx_buffers[NUMBER_TX_BUFFERS]{};
static uint8_t current_TX_buffer = 0;
static constexpr uint16_t TSAD_array[NUMBER_TX_BUFFERS] = {0x20, 0x24, 0x28, 0x2C};
static constexpr uint16_t TSD_array[NUMBER_TX_BUFFERS] = {0x10, 0x14, 0x18, 0x1C};

void turn_on()
{
	write_register8(RTL8139_PORT_CONFIG1, 0);
}

void software_rest()
{
	write_register8(RTL8139_PORT_CHIP_CMD, RTL8139_COMMAND_RESET);
	while (read_register8(RTL8139_PORT_CHIP_CMD) & RTL8139_COMMAND_RESET) {
	}
}

void setup_TX_buffers()
{
	for (size_t i = 0; i < NUMBER_TX_BUFFERS; i++) {
		tx_buffers[i] = valloc(0, MAX_TX_BUFFER_SIZE, PAGE_READWRITE | PAGE_CONTAGIOUS);
	}
}

void configure_rx()
{
	write_register32(RTL8139_PORT_RX_CONFIG,
	                 RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS | RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS |
	                     RTL8139_RX_CONFIG_ACCEPT_MULTICAST_PACKETS | RTL8139_RX_CONFIG_ACCEPT_BROADCAST_PACKETS |
	                     RTL8139_RX_CONFIG_EEPROM_WRAP);
}

void configure_RX_TX()
{
	write_register16(RTL8139_PORT_INTR_MASK, RTL8139_INTERRUPT_MASK_RX_OK | RTL8139_INTERRUPT_MASK_TX_OK);

	write_register32(RTL8139_PORT_RX_CONFIG, RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS |
	                                             RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_ADDRESS_PACKETS |
	                                             RTL8139_RX_CONFIG_ACCEPT_MULTICAST_PACKETS |
	                                             RTL8139_RX_CONFIG_ACCEPT_BROADCAST_PACKETS |
	                                             RTL8139_RX_CONFIG_EEPROM_WRAP);

	write_register8(RTL8139_PORT_CHIP_CMD, RTL8139_COMMAND_RX_ENABLE | RTL8139_COMMAND_TX_ENABLE);
}

void rx_interrupt_handler(ISRContextFrame& context)
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

void send_packet(const void* data, size_t len)
{
	if (len > MAX_TX_BUFFER_SIZE) {
		warn() << "Data too large to be sent!";
		return;
	}

	memcpy(tx_buffers[current_TX_buffer], data, len);
	write_register32(TSAD_array[current_TX_buffer], virtual_to_physical_address(tx_buffers[current_TX_buffer]));
	write_register32(TSD_array[current_TX_buffer], len);
	while (!(read_register32(TSD_array[current_TX_buffer]) & RTL8139_TX_STATUS_TOK)) {
	}
	current_TX_buffer = (current_TX_buffer + 1) % NUMBER_TX_BUFFERS;
}

void test_RTL8139_ethernet(GenericPCIDevice&& device)
{
	ports = device.BAR0().address();

	info() << "Device MAC address: " << Hex(read_register8(0)) << " " << Hex(read_register8(1)) << " "
	       << Hex(read_register8(2)) << " " << Hex(read_register8(3)) << " " << Hex(read_register8(4)) << " "
	       << Hex(read_register8(5)) << " ";

	device.enable_bus_mastering();
	device.enable_interrupts();

	turn_on();
	software_rest();
	configure_RX_TX();
	setup_TX_buffers();

	ISR::register_hardware_interrupt_handler(rx_interrupt_handler, device.interrupt_line());

	send_packet("hello there.", 10);
	Thread::sleep(1000);
	send_packet("hello there...", 25);
}

void write_register8(uint16_t address, uint8_t value)
{
	out8(ports + address, value);
}

void write_register16(uint16_t address, uint16_t value)
{
	out16(ports + address, value);
}

void write_register32(uint16_t address, uint32_t value)
{
	out32(ports + address, value);
}

uint8_t read_register8(uint16_t address)
{
	return in8(ports + address);
}

uint16_t read_register16(uint16_t address)
{
	return in16(ports + address);
}

uint32_t read_register32(uint16_t address)
{
	return in32(ports + address);
}