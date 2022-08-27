#pragma once
#include "network/network.h"
#include "network_definitions.h"
#include <result.h>
class TransportSession;

class TransportProtocol
{
  public:
	TransportProtocol(Network& network, SocketAddress address);
	Result<void> listen();
	Result<void> connect();
	Result<TransportSession&> accept(u16 port);

  protected:
	SocketAddress m_address;
	Reference<Network> m_network;
};

class TransportSession
{
  public:
	Result<void> send(const BufferView& data);
	Result<size_t> receive(u16 dest_port, Buffer& buffer);
	Result<size_t> receive(u16 dest_port, Buffer& buffer, SocketAddress& source_address);

  protected:
	SocketAddress m_address;
};
