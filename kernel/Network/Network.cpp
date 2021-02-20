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
	m_tcp.connect(IPv4Address{10, 0, 2, 2}, 80);
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