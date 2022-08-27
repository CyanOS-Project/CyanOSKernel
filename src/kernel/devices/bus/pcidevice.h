#pragma once
#include "pcibase_address_register.h"
#include <types.h>

class PCIDevice
{
  private:
	static constexpr u16 PCI_PORT_ADDRESS_PORT = 0xCF8;
	static constexpr u16 PCI_PORT_VALUE_PORT = 0xCFC;
	static constexpr u16 PCI_PORT_VENDOR_ID = 0x00;
	static constexpr u16 PCI_PORT_DEVICE_ID = 0x02;
	static constexpr u16 PCI_PORT_COMMAND = 0x04;
	static constexpr u16 PCI_PORT_STATUS = 0x06;
	static constexpr u16 PCI_PORT_REVISION_ID = 0x08;
	static constexpr u16 PCI_PORT_PROG_IF = 0x09;
	static constexpr u16 PCI_PORT_SUBCLASS = 0x0a;
	static constexpr u16 PCI_PORT_CLASS = 0x0b;
	static constexpr u16 PCI_PORT_CACHE_LINE_SIZE = 0x0c;
	static constexpr u16 PCI_PORT_LATENCY_TIMER = 0x0d;
	static constexpr u16 PCI_PORT_HEADER_TYPE = 0x0e;
	static constexpr u16 PCI_PORT_BIST = 0x0f;

	static constexpr u32 PCI_COMMAND_IO_SPACE = (1 << 0);
	static constexpr u32 PCI_COMMAND_MEMORY_SPACE = (1 << 1);
	static constexpr u32 PCI_COMMAND_BUS_MASTER = (1 << 2);
	static constexpr u32 PCI_COMMAND_SPECIAL_CYCLES = (1 << 3);
	static constexpr u32 PCI_COMMAND_INTERRUPT_DISABLE = (1 << 10);

	static constexpr u16 DEVICE_DOES_NOT_EXIST = 0xffff;

	uint8_t m_bus_number;
	uint8_t m_slot_number;
	uint8_t m_function_number;
	void select_port(uint8_t offset) const;

  protected:
	uint8_t read_config_word8(uint8_t offset) const;
	u16 read_config_word16(uint8_t offset) const;
	u32 read_config_word32(uint8_t offset) const;
	void write_config_word8(uint8_t offset, uint8_t data);
	void write_config_word16(uint8_t offset, u16 data);
	void write_config_word32(uint8_t offset, u32 data);

  public:
	enum class HeaderType : uint8_t
	{
		GeneralDevice = 0,
		PCIBridge = 1,
		CardBus = 3
	};
	PCIDevice(uint8_t bus_number, uint8_t slot_number, uint8_t function_number);
	uint8_t bus() const;
	uint8_t slot() const;
	uint8_t function() const;
	bool does_exist() const;
	u16 vendor_id() const;
	u16 device_id() const;
	u16 command() const;
	u16 status() const;
	uint8_t revision_id() const;
	uint8_t prog_if() const;
	uint8_t device_class() const;
	uint8_t device_subclass() const;
	uint8_t cache_line_size() const;
	uint8_t latency_timer() const;
	HeaderType header_type() const;
	bool has_multiple_functions() const;
	uint8_t bist() const;
	void enable_bus_mastering();
	void disable_bus_mastering();
	void enable_interrupts();
	void disable_interrupts();
};

class PCIBridge : public PCIDevice
{
  private:
	static constexpr u16 PCI_PORT_BAR0 = 0x10;
	static constexpr u16 PCI_PORT_BAR1 = 0x14;
	static constexpr u16 PCI_PORT_PRIMARY_BUS_NUMBER = 0x18;
	static constexpr u16 PCI_PORT_SECONDARY_BUS_NUMBER = 0x19;
	static constexpr u16 PCI_PORT_SUBORDINATE_BUS_NUMBER = 0x1A;
	static constexpr u16 PCI_PORT_SECONDARY_LATENCY_TIMER = 0x1B;

  public:
	PCIBridge(uint8_t bus_number, uint8_t slot_number, uint8_t function_number);
	PCIBridge(const PCIDevice& device);
	PCIBaseAddressRegister BAR0() const;
	PCIBaseAddressRegister BAR1() const;
	uint8_t primary_bus_number() const;
	uint8_t secondary_bus_number() const;
	uint8_t subordinate_bus_number() const;
	uint8_t secondary_latency_timer() const;
};

class GenericPCIDevice : public PCIDevice
{
  private:
	static constexpr u16 PCI_PORT_BAR0 = 0x10;
	static constexpr u16 PCI_PORT_BAR1 = 0x14;
	static constexpr u16 PCI_PORT_BAR2 = 0x18;
	static constexpr u16 PCI_PORT_BAR3 = 0x1C;
	static constexpr u16 PCI_PORT_BAR4 = 0x20;
	static constexpr u16 PCI_PORT_BAR5 = 0x24;
	static constexpr u16 PCI_PORT_INTERRUPT_LINE = 0x3C;
	static constexpr u16 PCI_PORT_INTERRUPT_PIN = 0x3D;

  public:
	GenericPCIDevice(uint8_t bus_number, uint8_t slot_number, uint8_t function_number);
	GenericPCIDevice(const PCIDevice& device);
	PCIBaseAddressRegister BAR0() const;
	PCIBaseAddressRegister BAR1() const;
	PCIBaseAddressRegister BAR2() const;
	PCIBaseAddressRegister BAR3() const;
	PCIBaseAddressRegister BAR4() const;
	PCIBaseAddressRegister BAR5() const;
	uint8_t interrupt_line() const;
	uint8_t interrupt_pin() const;
};