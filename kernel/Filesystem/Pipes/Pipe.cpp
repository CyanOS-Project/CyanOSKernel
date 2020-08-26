#include "Pipe.h"
#include "Tasking/ScopedLock.h"
#include "Utils/ErrorCodes.h"
#include "Utils/PathParser.h"

Pipe::Pipe(const StringView& name) :
    FSNode(name, 0, 0, NodeType::Pipe, BUFFER_SIZE),
    m_children{},
    m_buffer{BUFFER_SIZE},
    m_wait_queue{},
    m_lock{},
    m_readers{0},
    m_writers{0}
{
	m_lock.init();
	// FIXME: multiple writers, one reader.
}

Pipe::~Pipe()
{
}

Result<void> Pipe::open(OpenMode mode, OpenFlags flags)
{
	UNUSED(flags);
	if (m_readers != 0) {
		return ResultError(ERROR_INVALID_PARAMETERS);
	}

	if (mode == OpenMode::Read) {
		m_readers++;
	} else if (mode == OpenMode::Write) {
		m_writers++;
	} else {
		return ResultError(ERROR_INVALID_PARAMETERS);
	}

	UNUSED(mode);
	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::close(OpenMode mode)
{
	if (mode == OpenMode::Read) {
		ASSERT(m_readers);
		m_readers--;
	} else if (mode == OpenMode::Read) {
		ASSERT(m_writers);
		m_writers--;
	} else {
		ASSERT_NOT_REACHABLE();
	}

	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::read(void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);
	while (m_buffer.size() < size) {
		local_lock.release();
		Thread::current->wait_on(m_wait_queue);
		local_lock.acquire();
	}

	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size; i++) {
		_buf[i] = m_buffer.dequeue();
	}

	m_wait_queue.wake_up();
	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::write(const void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);
	if (m_buffer.available_size() < size) {
		local_lock.release();
		Thread::current->wait_on(m_wait_queue);
		local_lock.acquire();
	}

	const char* _buf = static_cast<const char*>(buff);
	for (size_t i = 0; i < size; i++) {
		m_buffer.queue(_buf[i]);
	}

	m_wait_queue.wake_up();
	return ResultError(ERROR_SUCCESS);
}

Result<bool> Pipe::can_read()
{
	return m_buffer.is_empty();
}

Result<bool> Pipe::can_write()
{
	return m_buffer.is_full();
}

Result<void> Pipe::remove()
{
	return ResultError(ERROR_INVALID_OPERATION);
}
