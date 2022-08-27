
#include "tcpsocket.h"
#include "network/network.h"

Vector<TCPSocket> TCPSocket::sockets{};

TCPSocket::TCPSocket(SocketAddress address) :
    FSNode{"", 0, 0, NodeType::IPSocket, 0},
    m_tcp{Network::default_network().tcp_provider()},
    m_address{address}
{
}

TCPSocket::TCPSocket(TCPSession& session) :
    FSNode{"", 0, 0, NodeType::IPSocket, 0},
    m_tcp{Network::default_network().tcp_provider()},
    m_session{session},
    m_address{session.address()}
{
}

TCPSocket::~TCPSocket() {}

Result<void> TCPSocket::open(FileDescription&)
{
	return {};
}

Result<void> TCPSocket::connect()
{
	if (m_type != SocketType::UnSpecfied)
		return ResultError{ERROR_INVALID_OPERATION};

	m_type = SocketType::Client;
	m_tcp->connect(m_address.ip, m_address.port);
	return {};
}

Result<void> TCPSocket::listen()
{
	if (m_type != SocketType::UnSpecfied)
		return ResultError{ERROR_INVALID_OPERATION};

	m_type = SocketType::Server;
	return {};
}

Result<FSNode&> TCPSocket::accept()
{
	if (m_type != SocketType::Server)
		return ResultError{ERROR_INVALID_OPERATION};

	auto result = m_tcp->accept(m_address.port);
	if (result)
		return ResultError{result.error()};

	return *sockets.emplace_back(result.value());
}

Result<size_t> TCPSocket::read(FileDescription&, BufferMutableView dest, size_t)
{
	if (!m_session.is_valid())
		return ResultError{ERROR_INVALID_OPERATION};
	return m_session->receive(dest);
}

Result<size_t> TCPSocket::write(FileDescription&, BufferView src, size_t)
{
	if (!m_session.is_valid())
		return ResultError{ERROR_INVALID_OPERATION};

	return m_session->send(src);
}

Result<void> TCPSocket::close(FileDescription&)
{
	return {};
}