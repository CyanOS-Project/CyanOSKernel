#pragma once
#include "Types.h"

class IPv4Address
{

  public:
	IPv4Address() = default;
	IPv4Address(const uint8_t* ip);
	IPv4Address(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3);
	IPv4Address(const IPv4Address& ip);
	IPv4Address& operator=(const IPv4Address& ip);
	uint8_t& operator[](uint8_t index);

	IPv4Address mask(const IPv4Address&) const;
	bool operator==(const IPv4Address&) const;
	bool operator!=(const IPv4Address&) const;
	uint8_t operator[](uint8_t index) const;
	void copy(uint8_t* data) const;

	static IPv4Address Broadcast;
	static IPv4Address Zero;

  private:
	static constexpr uint8_t IPv4_ELEMENTS_SIZE = 4;
	uint8_t m_ip[IPv4_ELEMENTS_SIZE] = {};
};
