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

Connection::~Connection() {}

Result<void> Connection::open(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<size_t> Connection::read(FileDescription& desc, void* buff, size_t offset, size_t size)
{
	UNUSED(offset);
	size_t ret = 0;
	if (desc.m_mode == OM_SERVER) {
		ret = read_server_buffer(desc, buff, size);
	} else if (desc.m_mode == OM_CLIENT) {
		ret = read_client_buffer(desc, buff, size);
	} else {
		ASSERT_NOT_REACHABLE();
	}
	return ret;
}

Result<size_t> Connection::write(FileDescription& desc, const void* buff, size_t offset, size_t size)
{
	UNUSED(offset);
	size_t ret = 0;
	if (desc.m_mode == OM_SERVER) {
		ret = write_server_buffer(desc, buff, size);
	} else if (desc.m_mode == OM_CLIENT) {
		ret = write_client_buffer(desc, buff, size);
	} else {
		ASSERT_NOT_REACHABLE();
	}
	return ret;
}

bool Connection::can_read(FileDescription& desc)
{
	if (desc.m_mode == OM_SERVER) {
		return !m_server_buffer.is_empty();
	} else if (desc.m_mode == OM_CLIENT) {
		return !m_client_buffer.is_empty();
	} else {
		ASSERT_NOT_REACHABLE();
		return false;
	}
}

bool Connection::can_write(FileDescription& desc)
{
	if (desc.m_mode == OM_SERVER) {
		return !m_client_buffer.is_full();
	} else if (desc.m_mode == OM_CLIENT) {
		return !m_server_buffer.is_full();
	} else {
		ASSERT_NOT_REACHABLE();
		return false;
	}
}

Result<void> Connection::close(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

size_t Connection::read_server_buffer(FileDescription& desc, void* buff, size_t size)
{
	ScopedLock local_lock(m_server_lock);

	while (!can_read(desc)) {
		local_lock.release();
		m_server_wait_queue.wait_on_event([&]() { return !can_read(desc); });
		local_lock.acquire();
	}

	size_t size_to_read = min(size, m_server_buffer.size());

	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size_to_read; i++) {
		_buf[i] = m_server_buffer.dequeue();
	}

	m_server_wait_queue.wake_up_all();

	return size_to_read;
}

size_t Connection::read_client_buffer(FileDescription& desc, void* buff, size_t size)
{
	ScopedLock local_lock(m_client_lock);

	while (!can_read(desc)) {
		local_lock.release();
		m_client_wait_queue.wait_on_event([&]() { return !can_read(desc); });
		local_lock.acquire();
	}

	size_t size_to_read = min(size, m_client_buffer.size());

	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size_to_read; i++) {
		_buf[i] = m_client_buffer.dequeue();
	}

	m_client_wait_queue.wake_up_all();

	return size_to_read;
}

size_t Connection::write_client_buffer(FileDescription& desc, const void* buff, size_t size)
{
	ScopedLock local_lock(m_server_lock);

	while (!can_write(desc)) {
		local_lock.release();
		m_server_wait_queue.wait_on_event([&]() { return !can_write(desc); });
		local_lock.acquire();
	}

	size_t size_to_write = min(size, m_server_buffer.available_size());

	const char* _buf = static_cast<const char*>(buff);
	for (size_t i = 0; i < size_to_write; i++) {
		m_server_buffer.queue(_buf[i]);
	}

	m_server_wait_queue.wake_up();

	return size_to_write;
}

size_t Connection::write_server_buffer(FileDescription& desc, const void* buff, size_t size)
{
	ScopedLock local_lock(m_client_lock);

	while (!can_write(desc)) {
		local_lock.release();
		m_client_wait_queue.wait_on_event([&]() { return !can_write(desc); });
		local_lock.acquire();
	}

	size_t size_to_write = min(size, m_client_buffer.available_size());

	const char* _buf = static_cast<const char*>(buff);
	for (size_t i = 0; i < size_to_write; i++) {
		m_client_buffer.queue(_buf[i]);
	}

	m_client_wait_queue.wake_up();

	return size_to_write;
}
