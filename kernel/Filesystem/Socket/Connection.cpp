#include "Connection.h"

Connection::Connection(const StringView& name) :
    FSNode{name, 0, 0, NodeType::Connection, 0},
    m_server_buffer{BUFFER_SIZE},
    m_client_buffer{BUFFER_SIZE},
    m_server_wait_queue{},
    m_client_wait_queue{},
    m_server_lock{},
    m_client_lock{}
{
}

Connection::~Connection()
{
}

Result<void> Connection::open(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> Connection::read(FileDescription&, void* buff, size_t offset, size_t size)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Connection::write(FileDescription&, const void* buff, size_t offset, size_t size)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<bool> Connection::can_read(FileDescription&)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<bool> Connection::can_write(FileDescription&)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Connection::close(FileDescription&)
{
	return ResultError(ERROR_INVALID_OPERATION);
}
