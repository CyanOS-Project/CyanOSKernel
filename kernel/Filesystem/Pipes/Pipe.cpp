#include "Pipe.h"
#include "Filesystem/FileDescription.h"
#include "Tasking/ScopedLock.h"
#include <ErrorCodes.h>

Pipe::Pipe(const StringView& name) :
    FSNode(name, 0, 0, NodeType::Pipe, BUFFER_SIZE),
    m_children{},
    m_buffer{BUFFER_SIZE},
    m_wait_queue{},
    m_lock{},
    m_readers{0},
    m_writers{0}
{
	// FIXME: multiple writers, one reader.
}

Pipe::~Pipe() {}

Result<void> Pipe::open(FileDescription& disc)
{
	/*if (m_readers != 0) {
	    return ResultError(ERROR_INVALID_PARAMETERS);
	}*/

	if (disc.m_mode == OpenMode::OM_READ) {
		m_readers++;
	} else if (disc.m_mode == OpenMode::OM_WRITE) {
		m_writers++;
	} else {
		return ResultError(ERROR_INVALID_PARAMETERS);
	}

	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::close(FileDescription& disc)
{
	if (disc.m_mode == OpenMode::OM_READ) {
		ASSERT(m_readers);
		m_readers--;
	} else if (disc.m_mode == OpenMode::OM_WRITE) {
		ASSERT(m_writers);
		m_writers--;
	} else {
		ASSERT_NOT_REACHABLE();
	}

	return ResultError(ERROR_SUCCESS);
}

Result<size_t> Pipe::read(FileDescription& desc, void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);

	if (size > BUFFER_SIZE) {
		return ResultError(ERROR_EOF);
	}

	m_wait_queue.wait_on_event([&]() { return !can_read(desc); }, local_lock);

	size_t size_to_read = min(size, m_buffer.size());

	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size_to_read; i++) {
		_buf[i] = m_buffer.dequeue();
	}

	m_wait_queue.wake_up();

	return size_to_read;
}

Result<size_t> Pipe::write(FileDescription& desc, const void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);

	if (size > BUFFER_SIZE) {
		return ResultError(ERROR_EOF);
	}

	m_wait_queue.wait_on_event([&]() { return !can_write(desc); }, local_lock);

	size_t size_to_write = min(size, m_buffer.available_size());

	const char* _buf = static_cast<const char*>(buff);
	for (size_t i = 0; i < size_to_write; i++) {
		m_buffer.queue(_buf[i]);
	}

	m_wait_queue.wake_up();

	return size_to_write;
}

bool Pipe::can_read(FileDescription&)
{
	return !m_buffer.is_empty();
}

bool Pipe::can_write(FileDescription&)
{
	return !m_buffer.is_full();
}

Result<void> Pipe::remove()
{
	return ResultError(ERROR_INVALID_OPERATION);
}
