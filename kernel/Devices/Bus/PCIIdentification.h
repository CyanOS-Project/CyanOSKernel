#pragma once
#include <Types.h>

const char* PCI_id_to_string(uint8_t class_id, uint8_t subclass_id);
const char* PCI_vendor_to_string(uint16_t vendor_id);
const char* PCI_device_id_to_string(uint16_t vendor_id, uint16_t device_id);
