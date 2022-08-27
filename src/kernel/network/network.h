#pragma once

#include "network/link_layer/network_adapter.h"
#include "network/network_layer/arp.h"
#include "network/network_layer/icmp.h"
#include "network/network_layer/ipv4.h"
#include "network/transport_layer/tcp.h"
#include "network/transport_layer/udp.h"
#include <types.h>

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

	static Network& default_network();
	~Network();

  private:
	NetworkAdapter& m_network_adapter;
	UDP m_udp;
	TCP m_tcp;
	ICMP m_icmp;
	ARP m_arp;
	IPv4 m_ipv4;
	static Network* default_network_ptr;
};
