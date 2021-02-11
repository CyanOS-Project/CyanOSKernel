#include "Network.h"
#include "Network/NetworkLayer/ICMP.h"
#include "Tasking/Thread.h"

Network::Network(const NetworkAdapter& network_adapter) : m_network_adapter{network_adapter}
{
	// ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.
	Thread::sleep(3000);
	ICMP(*this).send_echo_request(IPv4Address::Broadcast);
}