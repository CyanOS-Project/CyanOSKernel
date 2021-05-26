#pragma once
#include "Network/Network.h"
#include "NetworkDefinitions.h"
#include <Result.h>
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
