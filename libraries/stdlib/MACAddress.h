#pragma once
#include "Types.h"

class MACAddress
{
  public:
	MACAddress() = default;
	MACAddress(uint8_t* mac);
	MACAddress(const MACAddress&);
	MACAddress& operator=(const MACAddress&);
	bool operator==(const MACAddress& mac) const;
	bool operator!=(const MACAddress& mac) const;
	MACAddress mask(const MACAddress&) const;

  private:
	static constexpr uint8_t MAC_ELEMENTS_SIZE = 6;
	uint8_t m_mac[MAC_ELEMENTS_SIZE] = {};
};
