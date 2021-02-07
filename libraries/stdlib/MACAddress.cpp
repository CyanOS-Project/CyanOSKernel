#include "MACAddress.h"

MACAddress MACAddress::Broadcast = MACAddress{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
MACAddress MACAddress::Zero = MACAddress{0, 0, 0, 0, 0, 0};

MACAddress::MACAddress(uint8_t* mac)
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		m_mac[i] = mac[i];
	}
}

MACAddress::MACAddress(uint8_t mac0, uint8_t mac1, uint8_t mac2, uint8_t mac3, uint8_t mac4, uint8_t mac5)
{
	m_mac[0] = mac0;
	m_mac[1] = mac1;
	m_mac[2] = mac2;
	m_mac[3] = mac3;
	m_mac[4] = mac4;
	m_mac[5] = mac5;
}

MACAddress::MACAddress(const MACAddress& other)
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		m_mac[i] = other.m_mac[i];
	}
}

MACAddress& MACAddress::operator=(const MACAddress& other)
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		m_mac[i] = other.m_mac[i];
	}
	return *this;
}

bool MACAddress::operator==(const MACAddress& other) const
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		if (m_mac[i] != other.m_mac[i]) {
			return false;
		}
	}
	return true;
}

bool MACAddress::operator!=(const MACAddress& other) const
{
	return !operator==(other);
}

MACAddress MACAddress::mask(const MACAddress& mask) const
{
	MACAddress new_mac{};
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		new_mac.m_mac[i] = m_mac[i] & mask.m_mac[i];
	}
	return new_mac;
}

uint8_t& MACAddress::operator[](uint8_t index)
{
	return m_mac[index];
}

const uint8_t& MACAddress::operator[](uint8_t index) const
{
	return m_mac[index];
}

void MACAddress::copy(uint8_t* data) const
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		data[i] = m_mac[i];
	}
}