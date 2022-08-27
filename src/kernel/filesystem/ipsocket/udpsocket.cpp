
#include "udpsocket.h"
#include "network/network.h"

Vector<UDPSocket> UDPSocket::sockets{};

UDPSocket::UDPSocket(SocketAddress address) :
    FSNode{"", 0, 0, NodeType::IPSocket, 0},
    m_udp{Network::default_network().udp_provider()},
    m_address{address}
{
}

UDPSocket::~UDPSocket() {}

Result<void> UDPSocket::open(FileDescription&)
{
	return {};
}

Result<size_t> UDPSocket::read(FileDescription&, BufferMutableView dest, size_t)
{
	return m_udp->receive(m_address.port, dest);
}

Result<size_t> UDPSocket::write(FileDescription&, BufferView src, size_t)
{
	return m_udp->send(m_address, src);
}

Result<void> UDPSocket::close(FileDescription&)
{
	return {};
}