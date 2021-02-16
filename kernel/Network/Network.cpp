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

	// Thread::sleep(5000);

	/*ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.
	Thread::sleep(1000);
	ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.
	Thread::sleep(1000);
	ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.*/
	m_icmp.send_echo_request(IPv4Address{10, 0, 2, 2});
	// UDP().send(IPv4Address::Broadcast, 1234, Buffer{"Hello there", 10});
}

IPv4Address Network::device_ip() const
{
	return IPv4Address::Zero;
}

MACAddress Network::device_mac() const
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