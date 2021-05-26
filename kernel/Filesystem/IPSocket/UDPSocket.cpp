
#include "UDPSocket.h"
#include "Network/Network.h"

Vector<UDPSocket> UDPSocket::sockets{};

UDPSocket::UDPSocket(SocketAddress address) :
    FSNode{"", 0, 0, NodeType::IPSocket, 0},
    m_udp{Network::default_network().udp_provider()},
    m_address{address}
{
}

UDPSocket::~UDPSocket() {}

Result<void> UDPSocket::open(FileDescription&) {}

Result<size_t> UDPSocket::read(FileDescription&, BufferMutableView dest, size_t offset)
{
	// FIXME: handle offset.
	// return m_udp->receive(buff);
}

Result<size_t> UDPSocket::write(FileDescription&, BufferView src, size_t offset)
{
	// return m_udp->receive(buff);
}

Result<void> UDPSocket::close(FileDescription&)
{
	// delete node.
}