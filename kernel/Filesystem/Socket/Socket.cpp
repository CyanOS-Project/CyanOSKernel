#include "Socket.h"

Socket::Socket(const StringView& name) :
    FSNode{name, 0, 0, NodeType::Socket, 0},
    m_connections{},
    m_server_wait_queue{},
    m_connections_wait_queue{},
    m_lock{}
{
}

Socket::~Socket() {}

Result<void> Socket::open(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<bool> Socket::can_accept()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<FSNode&> Socket::accept()
{
	ScopedLock local_lock(m_lock);

	while (!m_pending_connections.size()) {
		m_lock.release();
		m_server_wait_queue.wait_on_event();
		m_lock.acquire();
	}
	Connection& new_connection = m_connections.emplace_back("");
	m_pending_connections.head().is_accepted = true;
	m_pending_connections.head().connection = &new_connection;
	m_connections_wait_queue.wake_up();

	return new_connection;
}

Result<FSNode&> Socket::connect()
{
	ScopedLock local_lock(m_lock);

	NewPendingConnection& pending_connection = m_pending_connections.emplace_back();
	m_server_wait_queue.wake_up();
	while (pending_connection.is_accepted == false) {
		m_lock.release();
		m_connections_wait_queue.wait_on_event();
		m_lock.acquire();
	}

	ASSERT(pending_connection.connection);
	return *pending_connection.connection;
}

Result<void> Socket::close(FileDescription&)
{
	// connection are destroyed by the destructor.
	return ResultError(ERROR_SUCCESS);
}
