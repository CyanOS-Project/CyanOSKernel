#include "MACAddress.h"

MACAddress::MACAddress(uint8_t* mac)
{
	for (size_t i = 0; i < MAC_ELEMENTS_SIZE; i++) {
		m_mac[i] = mac[i];
	}
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