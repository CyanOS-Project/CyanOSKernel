#pragma once

#include "Network/LinkLayer/NetworkAdapter.h"
#include "Network/NetworkLayer/ARP.h"
#include "Network/NetworkLayer/IPv4.h"
#include <Types.h>

class Network
{
  public:
	Network(const NetworkAdapter& network_adapter);
	~Network();

	const NetworkAdapter& m_network_adapter;
};
