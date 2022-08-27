#pragma once
#include "filesystem/fsnode.h"
#include "network/transport_layer/udp.h"
#include <null_or_reference.h>
#include <result.h>

class UDPSocket : public FSNode
{
  public:
	UDPSocket(SocketAddress);
	~UDPSocket();
	Result<void> open(FileDescription&) override;
	Result<size_t> read(FileDescription&, BufferMutableView dest, size_t offset) override;
	Result<size_t> write(FileDescription&, BufferView src, size_t offset) override;
	Result<void> close(FileDescription&) override;

  private:
	enum class SocketType
	{
		UnSpecfied,
		Server,
		Client
	};

	Reference<UDP> m_udp;
	SocketType m_type = SocketType::UnSpecfied;
	SocketAddress m_address;

	static Vector<UDPSocket> sockets;
};
