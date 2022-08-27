#include "domain_socket.h"

DomainSocket::DomainSocket(StringView name) :
    FSNode{name, 0, 0, NodeType::DomainSocket, 0},
    m_connections{},
    m_server_wait_queue{},
    m_connections_wait_queue{},
    m_lock{}
{
}

DomainSocket::~DomainSocket() {}

Result<void> DomainSocket::open(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

bool DomainSocket::can_accept()
{
	return false;
}

Result<FSNode&> DomainSocket::accept()
{
	ScopedLock local_lock(m_lock);

	m_server_wait_queue.wait_on_event([&]() { return !m_pending_connections.size(); }, local_lock);

	Connection& new_connection = m_connections.emplace_back("");
	m_pending_connections.first().is_accepted = true;
	m_pending_connections.last().connection = &new_connection;
	m_connections_wait_queue.wake_up();

	return new_connection;
}

Result<void> DomainSocket::connect()
{
	/*ScopedLock local_lock(m_lock);

	NewPendingConnection& pending_connection = m_pending_connections.emplace_back();
	m_server_wait_queue.wake_up();

	m_connections_wait_queue.wait_on_event([&]() { return pending_connection.is_accepted == false; }, local_lock);

	ASSERT(pending_connection.connection);
	return *pending_connection.connection;*/
}

Result<void> DomainSocket::close(FileDescription&)
{
	// connection are destroyed by the destructor.
	return ResultError(ERROR_SUCCESS);
}
