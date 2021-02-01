#include "PCI.h"
#include "Arch/x86/Asm.h"
#include "Devices/DebugPort/Logger.h"
#include "Devices/Ethernet/Intel_i217.h"
#include "PCIDevice.h"
#include "PCIIdentification.h"

void PCI::scan_function(Function<void(PCIDevice&)>& callback, uint8_t bus, uint8_t slot, uint8_t function)
{
	PCIDevice function_device{bus, slot, function};
	if (function_device.does_exist()) {
		callback(function_device);
		if (PCIDevice{bus, slot, function}.header_type() == PCIDevice::HeaderType::PCIBridge) {
			scan_bus(callback, PCIBridge{bus, slot, function}.secondary_bus_number());
		}
	}
}

void PCI::scan_slot(Function<void(PCIDevice&)>& callback, uint8_t bus, uint8_t slot)
{
	PCIDevice slot_device{bus, slot, 0};

	if (slot_device.does_exist()) {
		scan_function(callback, bus, slot, 0);
		if (slot_device.has_multiple_functions()) {
			for (size_t function = 1; function < 8; function++) {
				scan_function(callback, bus, slot, function);
			}
		}
	}
}

void PCI::scan_bus(Function<void(PCIDevice&)>& callback, uint8_t bus)
{
	for (size_t slot = 0; slot < 32; slot++) {
		scan_slot(callback, bus, slot);
	}
}

void PCI::scan_pci(Function<void(PCIDevice&)> callback)
{
	PCIDevice device(0, 0, 0);

	if (device.has_multiple_functions()) {
		for (size_t bus = 0; bus < 8; bus++) {
			scan_bus(callback, bus);
		}
	} else {
		scan_bus(callback, 0);
	}
}

void PCI::enumerate_pci_devices()
{
	scan_pci([](PCIDevice& device) {
		if (device.device_id() == 0x100E) {
			test_ethernet(GenericPCIDevice{device});
		}
	});
}