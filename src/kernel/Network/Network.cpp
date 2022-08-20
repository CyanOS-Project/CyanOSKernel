#include "Network.h"
#include "ApplicationLayer/DNS.h"
#include "Network/NetworkLayer/ICMP.h"
#include "Network/TransportLayer/DHCP.h"
#include "Network/TransportLayer/UDP.h"
#include "Tasking/Thread.h"

Network* Network::default_network_ptr = nullptr;

Network::Network(NetworkAdapter& network_adapter) :
    m_network_adapter{network_adapter},
    m_udp{*this},
    m_tcp{*this},
    m_icmp{*this},
    m_arp{*this},
    m_ipv4{*this}
{
	if (!default_network_ptr)
		default_network_ptr = this;
}

void Network::start()
{
	m_ipv4.start();

	// m_icmp.send_echo_request(IPv4Address{216, 58, 211, 196});
	// auto& connection = m_tcp.accept(80);

	auto client_thread = [this]() { DNS(m_udp, m_icmp, IPv4Address{8, 8, 8, 8}).reslove("www.google.com"); };

	Thread::create_thread(Thread::current->parent_process(), client_thread, ThreadPrivilege::Kernel);

	// m_tcp.connect(IPv4Address{10, 0, 2, 2}, 80);
}

IPv4Address Network::device_ip() const
{
	return m_ipv4.ip();
}

MACAddress Network::device_mac() const
{
	return m_network_adapter.mac();
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

Network& Network::default_network()
{
	if (!default_network_ptr) {
		warn() << "No network available.";
	}
	return *default_network_ptr;
}
