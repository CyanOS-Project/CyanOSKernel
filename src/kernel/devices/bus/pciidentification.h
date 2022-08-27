#pragma once
#include <types.h>

const char* PCI_id_to_string(uint8_t class_id, uint8_t subclass_id);
const char* PCI_vendor_to_string(u16 vendor_id);
const char* PCI_device_id_to_string(u16 vendor_id, u16 device_id);
