#include "PCI.h"
#include "Arch/x86/Asm.h"
#include "Devices/DebugPort/Logger.h"
#include "PCIIdentification.h"

void PCI::enumerate_pci_devices()
{

	for (size_t i = 0; i < 256; i++) {

		for (size_t j = 0; j < 32; j++) {
			for (size_t k = 0; k < 8; k++) {
				uint32_t id = read_pci_word16(i, j, k, PCI_VENDOR_ID);
				if (id != DEVICE_DOES_NOT_EXIST) {
					info() << "----------------------------------------------------";
					info() << "Class: "
					       << PCI_id_to_string(read_pci_word8(i, j, k, PCI_CLASS),
					                           read_pci_word8(i, j, k, PCI_SUBCLASS));
					info() << "Vendor: " << PCI_vendor_to_string(read_pci_word16(i, j, k, PCI_VENDOR_ID));
					info() << "Device: "
					       << PCI_device_id_to_string(read_pci_word16(i, j, k, PCI_VENDOR_ID),
					                                  read_pci_word16(i, j, k, PCI_DEVICE_ID));
					info() << "Bus Number     : " << Hex(i);
					info() << "Device Number  : " << Hex(j);
					info() << "Function Number: " << Hex(k);
					info() << "PCI_COMMAND    : " << Hex(read_pci_word16(i, j, k, PCI_COMMAND));
					info() << "PCI_STATUS     : " << Hex(read_pci_word16(i, j, k, PCI_STATUS));
					info() << "PCI_HEADER_TYPE: " << Hex(read_pci_word8(i, j, k, PCI_HEADER_TYPE));
				}
			}
		}
	}
}

uint8_t PCI::read_pci_word8(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	uint32_t address = (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_ADDRESS_PORT, address);
	return in8(PCI_VALUE_PORT + (offset & 0x3));
}

uint16_t PCI::read_pci_word16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	uint32_t address = (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_ADDRESS_PORT, address);
	return in16(PCI_VALUE_PORT + (offset & 0x2));
}

uint32_t PCI::read_pci_word32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	uint32_t address = (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_ADDRESS_PORT, address);
	return in32(PCI_VALUE_PORT);
}

void PCI::write_pci_word8(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint8_t data)
{
	uint32_t address = (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_ADDRESS_PORT, address);
	out8(PCI_VALUE_PORT + (offset & 0x3), data);
}

void PCI::write_pci_word16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t data)
{
	uint32_t address = (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_ADDRESS_PORT, address);
	out16(PCI_VALUE_PORT + (offset & 0x2), data);
}

void PCI::write_pci_word32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t data)
{
	uint32_t address = (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	out32(PCI_ADDRESS_PORT, address);
	out32(PCI_VALUE_PORT, data);
}
