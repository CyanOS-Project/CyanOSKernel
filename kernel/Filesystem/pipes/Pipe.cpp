#include "Pipe.h"
#include "utils/ErrorCodes.h"
#include "utils/PathParser.h"

Pipe& Pipe::root_node()
{
	return *new Pipe();
}

Pipe::Pipe(const char* name, Direction direction) :
    m_buffer{BUFFER_SIZE},
    m_reader{BUFFER_SIZE},
    m_writer{BUFFER_SIZE}, // FIXME: initial value is BUFFER_SIZE not 0
    m_wait_queue{},
    m_direction{direction}
{
	memcpy(m_filename, name, strlen(name) + 1);
}

Pipe::Pipe() :
    m_buffer{1},
    m_reader{1},
    m_writer{1}, // FIXME: initial value is BUFFER_SIZE not 0
    m_wait_queue{},
    m_direction{Direction::Reader}
{
}

Pipe::~Pipe()
{
}

Result<void> Pipe::read(void* buff, size_t offset, size_t size)
{
	// FIXME: check if type is not a folder
	UNUSED(offset);

	if (m_direction != Direction::Reader) {
		return ResultError(ERROR_INVALID_OPERATION);
	}
	if (m_buffer.size() < size) {
		Thread::current->wait_on(m_wait_queue);
	}

	m_wait_queue.wake_up();
	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size; i++) {
		m_buffer.queue(_buf[i]);
	}
	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::write(void* buff, size_t offset, size_t size)
{
	// FIXME: check if type is not a folder
	UNUSED(offset);

	if (m_direction != Direction::Writer) {
		return ResultError(ERROR_INVALID_OPERATION);
	}
	if (m_buffer.size() < size) {
		Thread::current->wait_on(m_wait_queue);
	}

	m_wait_queue.wake_up();
	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size; i++) {
		_buf[i] = m_buffer.dequeue();
	}

	return ResultError(ERROR_SUCCESS);
}

Result<bool> Pipe::can_read()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<bool> Pipe::can_write()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::remove()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> Pipe::create(const char* name, OpenMode mode, OpenFlags flags)
{
	UNUSED(flags);
	if (mode == OpenMode::ReadWrite) {
		return ResultError(ERROR_INVALID_OPERATION);
	}
	return m_children.emplace_back(name, mode == OpenMode::Read ? Direction::Reader : Direction::Writer);
}

Result<void> Pipe::mkdir(const char* name, int flags, int access)
{
	UNUSED(name);
	UNUSED(flags);
	UNUSED(access);
	PANIC("mkdir not implemented");
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::link(FSNode& node)
{
	UNUSED(node);
	PANIC("link not implemented");
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::unlink(FSNode& node)
{
	UNUSED(node);
	PANIC("unlink not implemented");
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> Pipe::dir_lookup(const char* file_name)
{
	UNUSED(file_name);
	PANIC("dir_lookup not implemented");
	return ResultError(ERROR_INVALID_PARAMETERS);
}
