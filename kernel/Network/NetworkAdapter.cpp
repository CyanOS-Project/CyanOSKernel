#include "NetworkAdapter.h"
#include "ARP.h"
#include "Devices/DebugPort/Logger.h"

void NetworkAdapter::handle_received_frame(ProtocolType type, const void* data, size_t size)
{
	if (type == ProtocolType::ARP) {
		warn() << "ARP Packet";
		ARP().handle_arp_packet(data, size);
	} else if (type == ProtocolType::IPv4) {
		warn() << "IPv4 Packet";
	} else {
		warn() << "Unkown Packet";
	}
}

const MACAddress& NetworkAdapter::MAC() const
{
	return m_mac;
}