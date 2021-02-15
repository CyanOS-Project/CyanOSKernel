#pragma once
#include "Types.h"

class MACAddress
{
  public:
	MACAddress() = default;
	MACAddress(const u8* mac);
	MACAddress(u8 mac0, u8 mac1, u8 mac2, u8 mac3, u8 mac4, u8 mac5);
	MACAddress(const MACAddress&);
	MACAddress& operator=(const MACAddress&);
	u8& operator[](u8 index);

	bool operator==(const MACAddress& mac) const;
	bool operator!=(const MACAddress& mac) const;
	MACAddress mask(const MACAddress&) const;
	u8 operator[](u8 index) const;
	void copy(u8* data) const;

	static MACAddress Broadcast;
	static MACAddress Zero;

  private:
	static constexpr u8 MAC_ELEMENTS_SIZE = 6;
	u8 m_mac[MAC_ELEMENTS_SIZE] = {};
};
