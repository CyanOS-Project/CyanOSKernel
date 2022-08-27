#include "macaddress.h"

MACAddress MACAddress::Broadcast = MACAddress{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
MACAddress MACAddress::Zero = MACAddress{0, 0, 0, 0, 0, 0};

MACAddress::MACAddress(const u8* mac)
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		m_mac[i] = mac[i];
	}
}

MACAddress::MACAddress(u8 mac0, u8 mac1, u8 mac2, u8 mac3, u8 mac4, u8 mac5)
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

MACAddress MACAddress::mask(MACAddress mask) const
{
	MACAddress new_mac{};
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		new_mac.m_mac[i] = m_mac[i] & mask.m_mac[i];
	}
	return new_mac;
}

u8& MACAddress::operator[](u8 index)
{
	return m_mac[index];
}

u8 MACAddress::operator[](u8 index) const
{
	return m_mac[index];
}

void MACAddress::copy(u8* data) const
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		data[i] = m_mac[i];
	}
}