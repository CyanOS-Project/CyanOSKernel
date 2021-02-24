#include "NetworkAdapter.h"
#include "Devices/DebugPort/Logger.h"
#include "Network/Network.h"
#include "Network/NetworkLayer/ARP.h"
#include "Network/NetworkLayer/IPv4.h"
#include "Tasking/Thread.h"

void NetworkAdapter::set_network_handler(Network& network)
{
	m_network = &network;
}

void NetworkAdapter::handle_received_frame(ProtocolType type, const BufferView& data)
{
	if (!m_network) {
		err() << "No network handler for this network adapter!";
	}

	auto frame_handler = [this, type, data = Buffer{data}]() mutable {
		if (type == ProtocolType::ARP) {
			m_network->arp_provider().handle_arp_packet(data);
		} else if (type == ProtocolType::IPv4) {
			m_network->ipv4_provider().handle_ip_packet(data);
		} else {
			warn() << "Unknown Packet";
		}
	};

	Thread::create_thread(Thread::current->parent_process(), frame_handler, ThreadPrivilege::Kernel);
}

MACAddress NetworkAdapter::MAC() const
{
	return m_mac;
}