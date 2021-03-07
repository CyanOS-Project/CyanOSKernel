#pragma once

#include "Network/LinkLayer/NetworkAdapter.h"
#include "Network/NetworkLayer/ARP.h"
#include "Network/NetworkLayer/ICMP.h"
#include "Network/NetworkLayer/IPv4.h"
#include "Network/TransportLayer/TCP.h"
#include "Network/TransportLayer/UDP.h"
#include <Types.h>

class Network
{
  public:
	Network(NetworkAdapter& network_adapter);
	void start();
	NetworkAdapter& network_adapter();
	IPv4& ipv4_provider();
	ARP& arp_provider();
	ICMP& icmp_provider();
	UDP& udp_provider();
	TCP& tcp_provider();
	IPv4Address device_ip() const;
	MACAddress device_mac() const;

	~Network();

  private:
	NetworkAdapter& m_network_adapter;
	UDP m_udp;
	TCP m_tcp;
	ICMP m_icmp;
	ARP m_arp;
	IPv4 m_ipv4;
};
