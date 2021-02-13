#pragma once

#include "Network/LinkLayer/NetworkAdapter.h"
#include "Network/NetworkLayer/ARP.h"
#include "Network/NetworkLayer/IPv4.h"
#include <Types.h>

class Network
{
  public:
	Network(const NetworkAdapter& network_adapter);
	IPv4Address IP() const;
	MACAddress MAC() const;
	~Network();

  private:
	const NetworkAdapter& m_network_adapter;
};
