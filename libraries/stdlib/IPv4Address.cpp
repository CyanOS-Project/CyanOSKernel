#include "IPv4Address.h"

IPv4Address IPv4Address::Broadcast = IPv4Address{0xFF, 0xFF, 0xFF, 0xFF};
IPv4Address IPv4Address::Zero = IPv4Address{0, 0, 0, 0};

IPv4Address::IPv4Address(const uint8_t* ip)
{
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		m_ip[i] = ip[i];
	}
}

IPv4Address::IPv4Address(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3)
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

IPv4Address IPv4Address::mask(const IPv4Address& mask) const
{
	IPv4Address new_ip{};
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		new_ip.m_ip[i] = m_ip[i] & mask.m_ip[i];
	}
	return new_ip;
}

uint8_t& IPv4Address::operator[](uint8_t index)
{
	return m_ip[index];
}

uint8_t IPv4Address::operator[](uint8_t index) const
{
	return m_ip[index];
}

void IPv4Address::copy(uint8_t* data) const
{
	for (size_t i = 0; i < IPv4_ELEMENTS_SIZE; i++) {
		data[i] = m_ip[i];
	}
}