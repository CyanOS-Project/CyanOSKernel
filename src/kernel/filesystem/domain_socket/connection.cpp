#include "connection.h"
#include "filesystem/file_description.h"

Connection::Connection(StringView name) :
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

Result<size_t> Connection::read(FileDescription& desc, BufferMutableView dest, size_t offset)
{
	UNUSED(offset);
	size_t ret = 0;
	if (desc.m_mode == OM_SERVER) {
		ret = read_server_buffer(desc, dest);
	} else if (desc.m_mode == OM_CLIENT) {
		ret = read_client_buffer(desc, dest);
	} else {
		ASSERT_NOT_REACHABLE();
	}
	return ret;
}

Result<size_t> Connection::write(FileDescription& desc, BufferView src, size_t offset)
{
	UNUSED(offset);
	size_t ret = 0;
	if (desc.m_mode == OM_SERVER) {
		ret = write_server_buffer(desc, src);
	} else if (desc.m_mode == OM_CLIENT) {
		ret = write_client_buffer(desc, src);
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

size_t Connection::read_server_buffer(FileDescription& desc, BufferMutableView dest)
{
	ScopedLock local_lock(m_server_lock);

	m_server_wait_queue.wait_on_event([&]() { return !can_read(desc); }, local_lock);

	size_t size_to_read = min(dest.size(), m_server_buffer.size());

	for (size_t i = 0; i < size_to_read; i++) {
		dest[i] = m_server_buffer.dequeue();
	}

	m_server_wait_queue.wake_up_all();

	return size_to_read;
}

size_t Connection::read_client_buffer(FileDescription& desc, BufferMutableView dest)
{
	ScopedLock local_lock(m_client_lock);

	m_client_wait_queue.wait_on_event([&]() { return !can_read(desc); }, local_lock);

	size_t size_to_read = min(dest.size(), m_client_buffer.size());

	for (size_t i = 0; i < size_to_read; i++) {
		dest[i] = m_client_buffer.dequeue();
	}

	m_client_wait_queue.wake_up_all();

	return size_to_read;
}

size_t Connection::write_client_buffer(FileDescription& desc, BufferView src)
{
	ScopedLock local_lock(m_server_lock);

	m_server_wait_queue.wait_on_event([&]() { return !can_write(desc); }, local_lock);

	size_t size_to_write = min(src.size(), m_server_buffer.available_size());

	for (size_t i = 0; i < size_to_write; i++) {
		m_server_buffer.queue(src[i]);
	}

	m_server_wait_queue.wake_up();

	return size_to_write;
}

size_t Connection::write_server_buffer(FileDescription& desc, BufferView src)
{
	ScopedLock local_lock(m_client_lock);

	m_client_wait_queue.wait_on_event([&]() { return !can_write(desc); }, local_lock);

	size_t size_to_write = min(src.size(), m_client_buffer.available_size());

	for (size_t i = 0; i < size_to_write; i++) {
		m_client_buffer.queue(src[i]);
	}

	m_client_wait_queue.wake_up();

	return size_to_write;
}
