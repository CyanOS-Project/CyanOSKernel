#pragma once
#include <Types.h>

class PCI
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

	enum class PCIClass {
		Unclassified = 0,
		MassStorageController,
		NetworkController,
		DisplayController,
		MultimediaController,
		MemoryController,
		BridgeDevice,
		SimpleCommunicationController,
		BaseSystemPeripheral,
		InputDeviceController,
		DockingStation,
		Processor,
		SerialBusController,
		WirelessController
	};
	static constexpr uint16_t DEVICE_DOES_NOT_EXIST = 0xffff;

  public:
	static uint8_t read_pci_word8(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	static uint16_t read_pci_word16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	static uint32_t read_pci_word32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
	static void write_pci_word8(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t data);
	static void write_pci_word16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint16_t data);
	static void write_pci_word32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t data);

	static void enumerate_pci_devices();
};
