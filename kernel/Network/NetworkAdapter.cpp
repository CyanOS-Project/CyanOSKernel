#include "NetworkAdapter.h"

void NetworkAdapter::handle_received_frame(ProtocolType type, const void* data, size_t size) {}

const MACAddress& NetworkAdapter::MAC() const
{
	return m_mac;
}