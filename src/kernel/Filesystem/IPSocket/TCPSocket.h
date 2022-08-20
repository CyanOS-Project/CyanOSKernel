#pragma once
#include "Filesystem/FSNode.h"
#include "Network/TransportLayer/TCP.h"
#include <NullOrReference.h>
#include <Result.h>

class TCPSocket : public FSNode
{
  public:
	TCPSocket(SocketAddress);
	TCPSocket(TCPSession&); // FIXME: should be private.
	~TCPSocket();
	Result<void> open(FileDescription&) override;
	Result<void> connect() override;
	Result<void> listen() override;
	Result<FSNode&> accept() override;
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

	Reference<TCP> m_tcp;
	SocketType m_type = SocketType::UnSpecfied;
	NullOrReference<TCPSession> m_session;
	SocketAddress m_address;

	static Vector<TCPSocket> sockets;
};
