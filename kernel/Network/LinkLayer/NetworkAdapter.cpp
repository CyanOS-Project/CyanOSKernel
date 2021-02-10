#include "NetworkAdapter.h"
#include "Devices/DebugPort/Logger.h"
#include "Network/NetworkLayer/ARP.h"

NetworkAdapter* NetworkAdapter::default_network_adapter = nullptr;

void NetworkAdapter::handle_received_frame(ProtocolType type, const BufferView& data)
{
	if (type == ProtocolType::ARP) {
		warn() << "ARP Packet";
		ARP::handle_arp_packet(data);
	} else if (type == ProtocolType::IPv4) {
		warn() << "IPv4 Packet";
	} else {
		warn() << "Unknown Packet";
	}
}

const MACAddress& NetworkAdapter::MAC() const
{
	return m_mac;
}