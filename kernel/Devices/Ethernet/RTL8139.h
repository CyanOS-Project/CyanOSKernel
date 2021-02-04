#pragma once
#include "Devices/Bus/PCIDevice.h"
#include "Types.h"

#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

#define RX_BUF_SIZE 8192

#define CAPR                 0x38
#define RX_READ_POINTER_MASK (~3)
#define ROK                  (1 << 0)
#define RER                  (1 << 1)
#define TOK                  (1 << 2)
#define TER                  (1 << 3)
#define TX_TOK               (1 << 15)

static constexpr uint8_t NUMBER_TX_BUFFERS = 4;
static constexpr uint16_t MAX_TX_BUFFER_SIZE = 0x700;

static constexpr uint16_t RTL8139_PORT_MAG0 = 0x00;       // Ethernet hardware address
static constexpr uint16_t RTL8139_PORT_MAR0 = 0x08;       // Multicast filter
static constexpr uint16_t RTL8139_PORT_TX_STATUS0 = 0x10; // Transmit status (Four 32bit registers)
static constexpr uint16_t RTL8139_PORT_TX_ADDR0 = 0x20;   // Tx descriptors (also four 32bit)
static constexpr uint16_t RTL8139_PORT_RX_BUF = 0x30;
static constexpr uint16_t RTL8139_PORT_RX_EARLY_CNT = 0x34;
static constexpr uint16_t RTL8139_PORT_RX_EARLY_STATUS = 0x36;
static constexpr uint16_t RTL8139_PORT_CHIP_CMD = 0x37;
static constexpr uint16_t RTL8139_PORT_RX_BUF_PTR = 0x38;
static constexpr uint16_t RTL8139_PORT_RX_BUF_ADDR = 0x3A;
static constexpr uint16_t RTL8139_PORT_INTR_MASK = 0x3C;
static constexpr uint16_t RTL8139_PORT_INTR_STATUS = 0x3E;
static constexpr uint16_t RTL8139_PORT_TX_CONFIG = 0x40;
static constexpr uint16_t RTL8139_PORT_RX_CONFIG = 0x44;
static constexpr uint16_t RTL8139_PORT_TIMER = 0x48;     // A general-purpose counter
static constexpr uint16_t RTL8139_PORT_RX_MISSED = 0x4C; // 24 bits valid, write clears
static constexpr uint16_t RTL8139_PORT_CFG9346 = 0x50;
static constexpr uint16_t RTL8139_PORT_CONFIG0 = 0x51;
static constexpr uint16_t RTL8139_PORT_CONFIG1 = 0x52;
static constexpr uint16_t RTL8139_PORT_FLASH_REG = 0x54;
static constexpr uint16_t RTL8139_PORT_GPPINDATA = 0x58;
static constexpr uint16_t RTL8139_PORT_GPPINDIR = 0x59;
static constexpr uint16_t RTL8139_PORT_MII_SMI = 0x5A;
static constexpr uint16_t RTL8139_PORT_HLT_CLK = 0x5B;
static constexpr uint16_t RTL8139_PORT_MULTI_INTR = 0x5C;
static constexpr uint16_t RTL8139_PORT_TX_SUMMARY = 0x60;
static constexpr uint16_t RTL8139_PORT_MII_BMCR = 0x62;
static constexpr uint16_t RTL8139_PORT_MII_BMSR = 0x64;
static constexpr uint16_t RTL8139_PORT_NWAYADVERT = 0x66;
static constexpr uint16_t RTL8139_PORT_NWAYLPAR = 0x68;
static constexpr uint16_t RTL8139_PORT_NWAYEXPANSION = 0x6A;
// Undocumented registers, but required for proper operation
static constexpr uint16_t RTL8139_PORT_FIFOTMS = 0x70; // FIFO Control and test
static constexpr uint16_t RTL8139_PORT_CSCR = 0x74;    // Chip Status and Configuration Register
static constexpr uint16_t RTL8139_PORT_PARA78 = 0x78;
static constexpr uint16_t RTL8139_PORT_PARA7c = 0x7c; // Magic transceiver parameter register

static constexpr uint16_t RTL8139_COMMAND_BUFFER_EMPTY = (1 << 0);
static constexpr uint16_t RTL8139_COMMAND_TX_ENABLE = (1 << 2);
static constexpr uint16_t RTL8139_COMMAND_RX_ENABLE = (1 << 3);
static constexpr uint16_t RTL8139_COMMAND_RESET = (1 << 4);

static constexpr uint16_t RTL8139_INTERRUPT_MASK_RX_OK = (1 << 0);
static constexpr uint16_t RTL8139_INTERRUPT_MASK_RX_ERROR = (1 << 1);
static constexpr uint16_t RTL8139_INTERRUPT_MASK_TX_OK = (1 << 2);
static constexpr uint16_t RTL8139_INTERRUPT_MASK_TX_ERROR = (1 << 3);

static constexpr uint16_t RTL8139_STATUS_MASK_RX_OK = (1 << 0);
static constexpr uint16_t RTL8139_STATUS_MASK_RX_ERROR = (1 << 1);
static constexpr uint16_t RTL8139_STATUS_MASK_TX_OK = (1 << 2);
static constexpr uint16_t RTL8139_STATUS_MASK_TX_ERROR = (1 << 3);

static constexpr uint32_t RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_ADDRESS_PACKETS = (1 << 0);
static constexpr uint32_t RTL8139_RX_CONFIG_ACCEPT_PHYSICAL_MATCH_PACKETS = (1 << 1);
static constexpr uint32_t RTL8139_RX_CONFIG_ACCEPT_MULTICAST_PACKETS = (1 << 2);
static constexpr uint32_t RTL8139_RX_CONFIG_ACCEPT_BROADCAST_PACKETS = (1 << 3);
static constexpr uint32_t RTL8139_RX_CONFIG_ACCEPT_RUNT_PACKETS = (1 << 4);
static constexpr uint32_t RTL8139_RX_CONFIG_ACCEPT_ERROR_PACKETS = (1 << 5);
static constexpr uint32_t RTL8139_RX_CONFIG_EEPROM_SELECT = (1 << 6);
static constexpr uint32_t RTL8139_RX_CONFIG_EEPROM_WRAP = (1 << 6);

typedef struct TX_Descriptor {
	uint32_t physical_addr;
	uint32_t packet_size;
} tx_desc_t;

void test_RTL8139_ethernet(GenericPCIDevice&& device);

void write_register8(uint16_t address, uint8_t value);
void write_register16(uint16_t address, uint16_t value);
void write_register32(uint16_t address, uint32_t value);
uint8_t read_register8(uint16_t address);
uint16_t read_register16(uint16_t address);
uint32_t read_register32(uint16_t address);