#include "Socket.h"

Socket::Socket(const StringView& name) :
    FSNode{name, 0, 0, NodeType::Socket, 0},
    m_connections{},
    m_wait_queue{},
    m_lock{}
{
}

Socket::~Socket()
{
}

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
		Thread::current->wait_on(m_wait_queue);
		m_lock.acquire();
	}
	// FIXME: move connection from m_pending_connections to m_connections
	return m_connections.tail();
}

Result<FSNode&> Socket::connect()
{
	FSNode& connection_node = m_pending_connections.emplace_back("");
	m_wait_queue.wake_up_all();
	// FIXME: wait until connection_node is accepted.
	return connection_node;
}

Result<void> Socket::close(FileDescription&)
{
	// connection are destroyed by the destructor.
	return ResultError(ERROR_SUCCESS);
}
