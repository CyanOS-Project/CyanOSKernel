#pragma once

#include <BufferView.h>
#include <IPv4Address.h>

class Network;
class TCP
{
  public:
	TCP(Network&);
	void send(const IPv4Address& dest_ip, u16 dest_port, const BufferView& data);
	void handle(const IPv4Address& src_ip, const BufferView& data);

	Network& m_network;
};
