#pragma once

#include "BufferView.h"
#include <MACAddress.h>
#include <Types.h>

enum class ProtocolType
{
	IPv4 = 0x0800,
	ARP = 0x0806,
	WakeOnLan = 0x0842
};

class Network;
class NetworkAdapter
{
  public:
	NetworkAdapter() = default;
	virtual void send_frame(ProtocolType type, const MACAddress& destination, const BufferView& data) = 0;
	void set_network_handler(Network& network);
	const MACAddress& MAC() const;

	static NetworkAdapter* default_network_adapter;

  protected:
	void handle_received_frame(ProtocolType type, const BufferView& data);

	MACAddress m_mac;
	Network* m_network;
};