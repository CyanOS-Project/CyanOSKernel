#include "Network.h"

Network::Network(const NetworkAdapter& network_adapter) : m_network_adapter{network_adapter}
{
	ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.

	IPv4::send_ip_packet(IPv4Address::Broadcast, IPv4Protocols::TCP, Buffer{10});
}