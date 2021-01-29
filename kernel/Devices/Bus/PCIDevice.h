#pragma once
#include <Types.h>

class PCIDevice
{
  private:
	uint8_t m_bus_number;
	uint8_t m_device_number;
	uint16_t m_vendor_id;
	uint16_t m_device_id;
	uint8_t m_class;
	uint8_t m_subclass;
	uint8_t m_prog_if;
	uint8_t m_revision_id;

  public:
	PCIDevice();
};
