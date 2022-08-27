#include "rtl8139.h"
#include "arch/x86/asm.h"
#include "arch/x86/isr.h"
#include "arch/x86/pic.h"
#include "devices/debug_port/logger.h"
#include "tasking/thread.h"
#include "virtual_memory/memory.h"

static void irq_handler(ISRContextFrame& context);
RTL8139* instance = nullptr;

RTL8139::RTL8139(GenericPCIDevice&& device) :
    GenericPCIDevice{move(device)},
    m_ports{BAR0().io_address()},
    m_mac{read_mac()}
{
	ASSERT(instance == 0);
	instance = this;

	enable_bus_mastering();
	enable_interrupts();

	turn_on();
	software_rest();
	start();
	setup_tx();
	setup_rx();

	ISR::register_hardware_interrupt_handler(irq_handler, interrupt_line());
}

MACAddress RTL8139::mac() const
{
	return m_mac;
}

void RTL8139::turn_on()
{
	write_register8(PORT_CONFIG1, 0);
}

void RTL8139::software_rest()
{
	write_register8(PORT_CHIP_CMD, COMMAND_RESET);
	while (read_register8(PORT_CHIP_CMD) & COMMAND_RESET) {
	}
}

void RTL8139::setup_tx()
{
	for (size_t i = 0; i < NUMBER_TX_BUFFERS; i++) {
		m_tx_buffers[i] = valloc(0, MAX_TX_BUFFER_SIZE, PAGE_READWRITE | PAGE_CONTAGIOUS);
		write_register32(TSAD_array[i], virtual_to_physical_address(m_tx_buffers[i]));
	}
}

void RTL8139::setup_rx()
{
	m_rx_buffer = reinterpret_cast<u8*>(valloc(0, MAX_RX_BUFFER_SIZE, PAGE_READWRITE | PAGE_CONTAGIOUS));
	write_register32(PORT_RX_BUF, virtual_to_physical_address(m_rx_buffer));
	write_register32(PORT_RX_CONFIG, RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS |
	                                     RX_CONFIG_ACCEPT_PHYSICAL_ADDRESS_PACKETS |
	                                     RX_CONFIG_ACCEPT_MULTICAST_PACKETS | RX_CONFIG_ACCEPT_BROADCAST_PACKETS |
	                                     RX_CONFIG_WRAP | RX_CONFIG_MAX_DMA_BURST_SIZE_256);
}

void RTL8139::start()
{
	write_register8(PORT_CHIP_CMD, COMMAND_RX_ENABLE | COMMAND_TX_ENABLE);

	write_register16(PORT_INTR_MASK, INTERRUPT_MASK_RX_OK | INTERRUPT_MASK_TX_OK);
}

void RTL8139::handle_rx()
{
	while (!(read_register8(PORT_CHIP_CMD) & COMMAND_BUFFER_EMPTY)) {

		RxPacket* received_packet = reinterpret_cast<RxPacket*>(m_rx_buffer + m_current_rx_pointer);

		if (is_packet_ok(received_packet->status) && received_packet->size) {
			handle_received_ethernet_frame(BufferView{received_packet->data, u16(received_packet->size - 4)});
		} else {
			err() << "Invalid Packet.";
		}

		m_current_rx_pointer =
		    (m_current_rx_pointer + received_packet->size + RX_PACKET_HEADER_SIZE + 3) & RX_READ_POINTER_MASK;

		if (m_current_rx_pointer > MAX_RX_BUFFER_SIZE) {
			m_current_rx_pointer -= MAX_RX_BUFFER_SIZE;
		}
		write_register16(PORT_RX_BUF_PTR, m_current_rx_pointer - RX_PAD);
	}
}

void RTL8139::handle_tx()
{
	// FIXME: release waitqueues for descriptors here.
}

void RTL8139::rx_tx_handler()
{

	u16 status = read_register16(PORT_INTR_STATUS);
	// warn() << "Network Interupt:";
	// info() << "ISR status : " << status;
	if (status & INTERRUPT_STATUS_TX_OK) {
		// info() << "Interrupt: Data has been sent!";
		handle_tx();
	}
	if (status & INTERRUPT_STATUS_RX_OK) {
		// info() << "Interrupt: Data has been received!";
		handle_rx();
	}

	write_register16(PORT_INTR_STATUS, status);
}

void RTL8139::send_ethernet_frame(const BufferView& data)
{
	m_rx_ports_semaphore.acquire();
	ScopedLock local_lock{m_lock};

	if (data.size() > MAX_TX_BUFFER_SIZE) {
		warn() << "Data too large to be sent!";
		return;
	}

	auto used_tx_buffer = m_current_tx_buffer;
	m_current_tx_buffer = (m_current_tx_buffer + 1) % NUMBER_TX_BUFFERS;

	data.copy_to(m_tx_buffers[used_tx_buffer], 0, data.size());
	write_register32(TSD_array[used_tx_buffer], data.size());

	while (!(read_register32(TSD_array[used_tx_buffer]) & TX_STATUS_TOK)) {
	}

	m_rx_ports_semaphore.release();
}

bool RTL8139::is_packet_ok(u16 status)
{
	if (status & RX_PACKET_STATUS_ROK) {
		return true;
	} else {
		return false;
	}
}

MACAddress RTL8139::read_mac()
{
	u32 mac1 = read_register32(0);
	u32 mac2 = read_register32(4);
	MACAddress mac{};
	mac[0] = (mac1 & 0xFF);
	mac[1] = (mac1 >> 8) & 0xFF;
	mac[2] = (mac1 >> 16) & 0xFF;
	mac[3] = (mac1 >> 24) & 0xFF;
	mac[4] = (mac2 & 0xFF);
	mac[5] = (mac2 >> 8) & 0xFF;
	return mac;
}

void RTL8139::write_register8(u16 address, u8 value)
{
	out8(m_ports + address, value);
}

void RTL8139::write_register16(u16 address, u16 value)
{
	out16(m_ports + address, value);
}

void RTL8139::write_register32(u16 address, u32 value)
{
	out32(m_ports + address, value);
}

u8 RTL8139::read_register8(u16 address)
{
	return in8(m_ports + address);
}

u16 RTL8139::read_register16(u16 address)
{
	return in16(m_ports + address);
}

u32 RTL8139::read_register32(u16 address)
{
	return in32(m_ports + address);
}

static void irq_handler(ISRContextFrame& context)
{
	UNUSED(context);
	ASSERT(instance);
	instance->rx_tx_handler();
}