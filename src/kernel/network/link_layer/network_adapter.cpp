#include "network_adapter.h"
#include "devices/debug_port/logger.h"
#include "network/network.h"
#include "network/network_layer/arp.h"
#include "network/network_layer/ipv4.h"
#include "tasking/thread.h"

void NetworkAdapter::set_network_handler(Network& network)
{
	m_network = &network;
}

void NetworkAdapter::handle_received_frame(ProtocolType type, const BufferView& data)
{
	if (!m_network) {
		err() << "No network handler for this network adapter!";
	}

	auto frame_handler = [this, type, data = Buffer(data)]() mutable {
		if (type == ProtocolType::ARP) {
			m_network->arp_provider().handle(data);
		} else if (type == ProtocolType::IPv4) {
			m_network->ipv4_provider().handle(data);
		} else {
			warn() << "Unknown Packet";
		}
	};

	Thread::create_thread(Thread::current->parent_process(), move(frame_handler), ThreadPrivilege::Kernel);
}