#include "IPv4Address.h"

IPv4Address IPv4Address::Broadcast = IPv4Address{0xFF, 0xFF, 0xFF, 0xFF};
IPv4Address IPv4Address::Zero = IPv4Address{0, 0, 0, 0};

IPv4Address::IPv4Address(const u8* ip)
{
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		m_ip[i] = ip[i];
	}
}
IPv4Address::IPv4Address(u32 ip)
{
	u8* c_ip = reinterpret_cast<u8*>(&ip);
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		m_ip[i] = c_ip[i];
	}
}

IPv4Address::IPv4Address(u8 ip0, u8 ip1, u8 ip2, u8 ip3)
{
	m_ip[0] = ip0;
	m_ip[1] = ip1;
	m_ip[2] = ip2;
	m_ip[3] = ip3;
}

IPv4Address::IPv4Address(const IPv4Address& other)
{
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		m_ip[i] = other.m_ip[i];
	}
}

IPv4Address& IPv4Address::operator=(const IPv4Address& other)
{
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		m_ip[i] = other.m_ip[i];
	}
	return *this;
}

bool IPv4Address::operator==(const IPv4Address& other) const
{
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		if (m_ip[i] != other.m_ip[i]) {
			return false;
		}
	}
	return true;
}

bool IPv4Address::operator!=(const IPv4Address& other) const
{
	return !operator==(other);
}

IPv4Address IPv4Address::mask(IPv4Address mask) const
{
	IPv4Address new_ip{};
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		new_ip.m_ip[i] = m_ip[i] & mask.m_ip[i];
	}
	return new_ip;
}

u8& IPv4Address::operator[](u8 index)
{
	return m_ip[index];
}

u8 IPv4Address::operator[](u8 index) const
{
	return m_ip[index];
}

void IPv4Address::copy(u8* data) const
{
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		data[i] = m_ip[i];
	}
}