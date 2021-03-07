#pragma once
#include "Devices/Bus/PCIDevice.h"
#include "Network/LinkLayer/EthernetNetworkAdapter.h"
#include "Tasking/Semaphore.h"
#include "Tasking/SpinLock.h"
#include "Types.h"

class RTL8139 : public EthernetNetworkAdapter, public GenericPCIDevice
{
  public:
	NON_COPYABLE(RTL8139)
	NON_MOVABLE(RTL8139)
	RTL8139(GenericPCIDevice&& device);
	MACAddress mac() const override;
	~RTL8139();
	void rx_tx_handler();

	static constexpr u16 VENDOR_ID = 0x10EC;
	static constexpr u16 DEVICE_ID = 0x8139;

  private:
	static constexpr u8 NUMBER_TX_BUFFERS = 4;
	static constexpr u16 MAX_TX_BUFFER_SIZE = 0x700;
	static constexpr u16 MAX_RX_BUFFER_SIZE = 8192 + 1536 + 16;

	static constexpr u16 PORT_MAG0 = 0x00;
	static constexpr u16 PORT_MAR0 = 0x08;
	static constexpr u16 PORT_TX_STATUS0 = 0x10;
	static constexpr u16 PORT_TX_STATUS1 = 0x14;
	static constexpr u16 PORT_TX_STATUS2 = 0x18;
	static constexpr u16 PORT_TX_STATUS3 = 0x1C;
	static constexpr u16 PORT_TX_ADDR0 = 0x20;
	static constexpr u16 PORT_TX_ADDR1 = 0x24;
	static constexpr u16 PORT_TX_ADDR2 = 0x28;
	static constexpr u16 PORT_TX_ADDR3 = 0x2C;
	static constexpr u16 PORT_RX_BUF = 0x30;
	static constexpr u16 PORT_RX_EARLY_CNT = 0x34;
	static constexpr u16 PORT_RX_EARLY_STATUS = 0x36;
	static constexpr u16 PORT_CHIP_CMD = 0x37;
	static constexpr u16 PORT_RX_BUF_PTR = 0x38;
	static constexpr u16 PORT_RX_BUF_ADDR = 0x3A;
	static constexpr u16 PORT_INTR_MASK = 0x3C;
	static constexpr u16 PORT_INTR_STATUS = 0x3E;
	static constexpr u16 PORT_TX_CONFIG = 0x40;
	static constexpr u16 PORT_RX_CONFIG = 0x44;
	static constexpr u16 PORT_TIMER = 0x48;
	static constexpr u16 PORT_RX_MISSED = 0x4C;
	static constexpr u16 PORT_CFG9346 = 0x50;
	static constexpr u16 PORT_CONFIG0 = 0x51;
	static constexpr u16 PORT_CONFIG1 = 0x52;
	static constexpr u16 PORT_FLASH_REG = 0x54;
	static constexpr u16 PORT_GPPINDATA = 0x58;
	static constexpr u16 PORT_GPPINDIR = 0x59;
	static constexpr u16 PORT_MII_SMI = 0x5A;
	static constexpr u16 PORT_HLT_CLK = 0x5B;
	static constexpr u16 PORT_MULTI_INTR = 0x5C;
	static constexpr u16 PORT_TX_SUMMARY = 0x60;
	static constexpr u16 PORT_MII_BMCR = 0x62;
	static constexpr u16 PORT_MII_BMSR = 0x64;
	static constexpr u16 PORT_NWAYADVERT = 0x66;
	static constexpr u16 PORT_NWAYLPAR = 0x68;
	static constexpr u16 PORT_NWAYEXPANSION = 0x6A;

	static constexpr u16 COMMAND_BUFFER_EMPTY = Bit(0);
	static constexpr u16 COMMAND_TX_ENABLE = Bit(2);
	static constexpr u16 COMMAND_RX_ENABLE = Bit(3);
	static constexpr u16 COMMAND_RESET = Bit(4);

	static constexpr u16 INTERRUPT_MASK_RX_OK = Bit(0);
	static constexpr u16 INTERRUPT_MASK_RX_ERROR = Bit(1);
	static constexpr u16 INTERRUPT_MASK_TX_OK = Bit(2);
	static constexpr u16 INTERRUPT_MASK_TX_ERROR = Bit(3);
	static constexpr u16 INTERRUPT_MASK_BUFFER_OVERFLOW = Bit(4);
	static constexpr u16 INTERRUPT_MASK_PACKET_UNDERRUN = Bit(5);
	static constexpr u16 INTERRUPT_MASK_RX_FIFO_OVERFLOW = Bit(6);
	static constexpr u16 INTERRUPT_MASK_DESCRIPTOR_UNAVAILABLE = Bit(7);
	static constexpr u16 INTERRUPT_MASK_SOFTWARE_INTERRUPT = Bit(8);
	static constexpr u16 INTERRUPT_MASK_CABLE_LEN_CHANGE = Bit(13);
	static constexpr u16 INTERRUPT_MASK_TIMEOUT = Bit(14);
	static constexpr u16 INTERRUPT_MASK_SYSTEM_ERROR = Bit(15);

	static constexpr u16 INTERRUPT_STATUS_RX_OK = Bit(0);
	static constexpr u16 INTERRUPT_STATUS_RX_ERROR = Bit(1);
	static constexpr u16 INTERRUPT_STATUS_TX_OK = Bit(2);
	static constexpr u16 INTERRUPT_STATUS_TX_ERROR = Bit(3);

	static constexpr u32 RX_CONFIG_ACCEPT_PHYSICAL_ADDRESS_PACKETS = Bit(0);
	static constexpr u32 RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS = Bit(1);
	static constexpr u32 RX_CONFIG_ACCEPT_MULTICAST_PACKETS = Bit(2);
	static constexpr u32 RX_CONFIG_ACCEPT_BROADCAST_PACKETS = Bit(3);
	static constexpr u32 RX_CONFIG_ACCEPT_RUNT_PACKETS = Bit(4);
	static constexpr u32 RX_CONFIG_ACCEPT_ERROR_PACKETS = Bit(5);
	static constexpr u32 RX_CONFIG_EEPROM_SELECT = Bit(6);
	static constexpr u32 RX_CONFIG_WRAP = Bit(7);
	static constexpr u32 RX_CONFIG_MAX_DMA_BURST_SIZE_256 = (4 << 8);

	static constexpr u32 RX_PACKET_STATUS_ROK = Bit(0);

	static constexpr u32 TX_STATUS_OWN = Bit(13);
	static constexpr u32 TX_STATUS_TUN = Bit(14);
	static constexpr u32 TX_STATUS_TOK = Bit(15);

	static constexpr u16 TSD_array[NUMBER_TX_BUFFERS] = {PORT_TX_STATUS0, PORT_TX_STATUS1, PORT_TX_STATUS2,
	                                                     PORT_TX_STATUS3};
	static constexpr u16 TSAD_array[NUMBER_TX_BUFFERS] = {PORT_TX_ADDR0, PORT_TX_ADDR1, PORT_TX_ADDR2, PORT_TX_ADDR3};

	static constexpr u32 RX_READ_POINTER_MASK = ~3;
	static constexpr u32 RX_PAD = 0x10;

	struct RxPacket {
		u16 status;
		u16 size;
		u8 data[1];
	} __attribute__((packed));
	static constexpr u32 RX_PACKET_HEADER_SIZE = offsetof(RxPacket, data);

	u16 m_ports = 0;
	MACAddress m_mac;
	u8 m_current_tx_buffer = 0;
	u32 m_current_rx_pointer = 0;
	void* m_tx_buffers[NUMBER_TX_BUFFERS] = {};
	u8* m_rx_buffer = nullptr;
	Spinlock m_lock;
	Semaphore m_rx_ports_semaphore{4};

	void turn_on();
	void software_rest();
	void setup_tx();
	void setup_rx();
	void start();
	void handle_rx();
	void handle_tx();
	void send_ethernet_frame(const BufferView& data) override;
	bool is_packet_ok(u16);
	MACAddress read_mac();

	void write_register8(u16 address, u8 value);
	void write_register16(u16 address, u16 value);
	void write_register32(u16 address, u32 value);
	u8 read_register8(u16 address);
	u16 read_register16(u16 address);
	u32 read_register32(u16 address);
};
