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

class NetworkAdapter
{
  public:
	NetworkAdapter() = default;
	virtual void send_frame(ProtocolType type, const MACAddress& destination, const BufferView& data) = 0;
	const MACAddress& MAC() const;

  protected:
	void handle_received_frame(ProtocolType type, const BufferView& data);

	MACAddress m_mac;
};