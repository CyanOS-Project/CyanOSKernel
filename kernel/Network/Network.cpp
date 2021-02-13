#include "Network.h"
#include "Network/NetworkLayer/ICMP.h"
#include "Network/TransportLayer/DHCP.h"
#include "Network/TransportLayer/UDP.h"
#include "Tasking/Thread.h"

Network::Network(const NetworkAdapter& network_adapter) : m_network_adapter{network_adapter}
{
	Thread::sleep(5000);

	/*ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.
	Thread::sleep(1000);
	ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.
	Thread::sleep(1000);
	ARP::mac_address_lookup(IPv4Address{10, 0, 2, 2}); // IP 10.0.2.2 is Firewall/DHCP server in Qemu.*/
	// ICMP(*this).send_echo_request(IPv4Address{IPv4Address::Broadcast});
	// UDP().send(IPv4Address::Broadcast, 1234, Buffer{"Hello there", 10});
	DHCP(*this).get_my_ip();
}

IPv4Address Network::IP() const
{
	return IPv4Address::Zero;
}

MACAddress Network::MAC() const
{
	return m_network_adapter.MAC();
}
