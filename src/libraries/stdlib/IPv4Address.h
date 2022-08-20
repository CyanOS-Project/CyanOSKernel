#pragma once
#include "Types.h"

class IPv4Address
{

  public:
	IPv4Address() = default;
	IPv4Address(const u8* ip);
	IPv4Address(u32 ip);
	IPv4Address(u8 ip0, u8 ip1, u8 ip2, u8 ip3);
	IPv4Address(const IPv4Address& ip);
	IPv4Address& operator=(const IPv4Address& ip);
	u8& operator[](u8 index);

	IPv4Address mask(IPv4Address) const;
	bool operator==(const IPv4Address&) const;
	bool operator!=(const IPv4Address&) const;
	u8 operator[](u8 index) const;
	void copy(u8* data) const;

	static IPv4Address Broadcast;
	static IPv4Address Zero;

  private:
	static constexpr u8 IPv4_ELEMENTS_SIZE = 4;
	u8 m_ip[IPv4_ELEMENTS_SIZE] = {};
};
