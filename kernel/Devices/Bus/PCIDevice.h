#pragma once
#include "PCIBaseAddressRegister.h"
#include <Types.h>

class PCIDevice
{
  private:
	static constexpr uint16_t PCI_ADDRESS_PORT = 0xCF8;
	static constexpr uint16_t PCI_VALUE_PORT = 0xCFC;
	static constexpr uint16_t PCI_VENDOR_ID = 0x00;
	static constexpr uint16_t PCI_DEVICE_ID = 0x02;
	static constexpr uint16_t PCI_COMMAND = 0x04;
	static constexpr uint16_t PCI_STATUS = 0x06;
	static constexpr uint16_t PCI_REVISION_ID = 0x08;
	static constexpr uint16_t PCI_PROG_IF = 0x09;
	static constexpr uint16_t PCI_SUBCLASS = 0x0a;
	static constexpr uint16_t PCI_CLASS = 0x0b;
	static constexpr uint16_t PCI_CACHE_LINE_SIZE = 0x0c;
	static constexpr uint16_t PCI_LATENCY_TIMER = 0x0d;
	static constexpr uint16_t PCI_HEADER_TYPE = 0x0e;
	static constexpr uint16_t PCI_BIST = 0x0f;

	static constexpr uint16_t DEVICE_DOES_NOT_EXIST = 0xffff;

	uint8_t m_bus_number;
	uint8_t m_slot_number;
	uint8_t m_function_number;
	void select_device(uint8_t offset) const;

  protected:
	uint8_t read_config_word8(uint8_t offset) const;
	uint16_t read_config_word16(uint8_t offset) const;
	uint32_t read_config_word32(uint8_t offset) const;
	void write_config_word8(uint8_t offset, uint8_t data);
	void write_config_word16(uint8_t offset, uint16_t data);
	void write_config_word32(uint8_t offset, uint32_t data);

  public:
	enum class HeaderType : uint8_t { GeneralDevice = 0, PCIBridge = 1, CardBus = 3 };
	PCIDevice(uint8_t bus_number, uint8_t slot_number, uint8_t function_number);
	uint8_t bus() const;
	uint8_t slot() const;
	uint8_t function() const;
	bool does_exist() const;
	uint16_t vendor_id() const;
	uint16_t device_id() const;
	uint16_t command() const;
	uint16_t status() const;
	uint8_t revision_id() const;
	uint8_t prog_if() const;
	uint8_t device_class() const;
	uint8_t device_subclass() const;
	uint8_t cache_line_size() const;
	uint8_t latency_timer() const;
	HeaderType header_type() const;
	bool has_multiple_functions() const;
	uint8_t bist() const;
};

class PCIBridge : public PCIDevice
{
  private:
	static constexpr uint16_t PCI_BAR0 = 0x10;
	static constexpr uint16_t PCI_BAR1 = 0x14;
	static constexpr uint16_t PCI_PRIMARY_BUS_NUMBER = 0x18;
	static constexpr uint16_t PCI_SECONDARY_BUS_NUMBER = 0x19;
	static constexpr uint16_t PCI_SUBORDINATE_BUS_NUMBER = 0x1A;
	static constexpr uint16_t PCI_SECONDARY_LATENCY_TIMER = 0x1B;

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
	static constexpr uint16_t PCI_BAR0 = 0x10;
	static constexpr uint16_t PCI_BAR1 = 0x14;
	static constexpr uint16_t PCI_BAR2 = 0x18;
	static constexpr uint16_t PCI_BAR3 = 0x1C;
	static constexpr uint16_t PCI_BAR4 = 0x20;
	static constexpr uint16_t PCI_BAR5 = 0x24;
	static constexpr uint16_t PCI_INTERRUPT_LINE = 0x3C;
	static constexpr uint16_t PCI_INTERRUPT_PIN = 0x3D;

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