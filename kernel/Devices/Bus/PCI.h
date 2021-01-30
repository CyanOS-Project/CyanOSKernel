#pragma once
#include "PCIDevice.h"
#include <Function.h>
#include <Types.h>

class PCI
{
  private:
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

  public:
	static void scan_function(Function<void(PCIDevice&)>&, uint8_t bus, uint8_t slot, uint8_t function);
	static void scan_slot(Function<void(PCIDevice&)>&, uint8_t bus, uint8_t slot);
	static void scan_bus(Function<void(PCIDevice&)>&, uint8_t bus);
	static void scan_pci(Function<void(PCIDevice&)> callback);
	static void enumerate_pci_devices();
};
