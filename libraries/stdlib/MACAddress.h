#pragma once
#include "Types.h"

class MACAddress
{
  public:
	MACAddress() = default;
	MACAddress(uint8_t* mac);
	MACAddress(uint8_t mac0, uint8_t mac1, uint8_t mac2, uint8_t mac3, uint8_t mac4, uint8_t mac5);
	MACAddress(const MACAddress&);
	MACAddress& operator=(const MACAddress&);
	uint8_t& operator[](uint8_t index);
	bool operator==(const MACAddress& mac) const;
	bool operator!=(const MACAddress& mac) const;
	MACAddress mask(const MACAddress&) const;
	const uint8_t& operator[](uint8_t index) const;
	void copy(uint8_t* data) const;

	static MACAddress Broadcast;
	static MACAddress Zero;

  private:
	static constexpr uint8_t MAC_ELEMENTS_SIZE = 6;
	uint8_t m_mac[MAC_ELEMENTS_SIZE] = {};
};
