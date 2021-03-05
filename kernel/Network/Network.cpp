#include "Network.h"
#include "Network/NetworkLayer/ICMP.h"
#include "Network/TransportLayer/DHCP.h"
#include "Network/TransportLayer/UDP.h"
#include "Tasking/Thread.h"

Network::Network(NetworkAdapter& network_adapter) :
    m_network_adapter{network_adapter},
    m_udp{*this},
    m_tcp{*this},
    m_icmp{*this},
    m_arp{*this},
    m_ipv4{*this}
{
}

void Network::start()
{
	m_ipv4.start();

	// m_icmp.send_echo_request(IPv4Address{10, 0, 2, 2});
	// auto& connection = m_tcp.accept(80);

	auto client_thread = [this]() {
		// auto& connection = m_tcp.connect(IPv4Address{216, 58, 198, 78}, 80);
		auto& connection = m_tcp.accept(80);
		Buffer buf{100};
		while (1) {
			if (auto error = connection.receive(buf)) {
				info() << "Error: " << error.error();
				break;
			}
			connection.send(BufferView{"you sent something", 19});
			info() << "Message: " << (char*)(buf.ptr());
			memset(buf.ptr(), 0, buf.size());
			warn() << "---------------------------";
		}
	};

	Thread::create_thread(Thread::current->parent_process(), client_thread, ThreadPrivilege::Kernel);

	// m_tcp.connect(IPv4Address{10, 0, 2, 2}, 80);
}

IPv4Address Network::device_ip()
{
	return m_ipv4.IP();
}

MACAddress Network::device_mac()
{
	return m_network_adapter.MAC();
}

NetworkAdapter& Network::network_adapter()
{
	return m_network_adapter;
}

IPv4& Network::ipv4_provider()
{
	return m_ipv4;
}

ARP& Network::arp_provider()
{
	return m_arp;
}

ICMP& Network::icmp_provider()
{
	return m_icmp;
}

UDP& Network::udp_provider()
{
	return m_udp;
}

TCP& Network::tcp_provider()
{
	return m_tcp;
}