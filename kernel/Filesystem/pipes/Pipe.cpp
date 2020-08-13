#include "Pipe.h"
#include "utils/ErrorCodes.h"
#include "utils/PathParser.h"

Pipe& Pipe::root_node()
{
	return *new Pipe();
}

Pipe::Pipe(const char* name) :
    FSNode(0, 0, BUFFER_SIZE),
    m_buffer{BUFFER_SIZE},
    m_reader{BUFFER_SIZE},
    m_writer{BUFFER_SIZE}, // FIXME: initial value is BUFFER_SIZE not 0
    m_wait_queue{}
// m_direction{direction}
{
	memcpy(m_filename, name, strlen(name) + 1);
}

Pipe::Pipe() :
    FSNode(0, 0, 0),
    m_buffer{1},
    m_reader{1},
    m_writer{1}, // FIXME: initial value is BUFFER_SIZE not 0
    m_wait_queue{}
// m_direction{Direction::Reader}
{
}

Pipe::~Pipe()
{
}

Result<void> Pipe::read(void* buff, size_t offset, size_t size)
{
	// FIXME: check if type is not a folder
	UNUSED(offset);

	/*if (m_direction != Direction::Reader) {
	    return ResultError(ERROR_INVALID_OPERATION);
	}*/
	if (m_buffer.size() < size) {
		Thread::current->wait_on(m_wait_queue);
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
	// FIXME: check if type is not a folder
	UNUSED(offset);

	/*if (m_direction != Direction::Writer) {
	    return ResultError(ERROR_INVALID_OPERATION);
	}*/
	if (m_buffer.available_size() < size) {
		Thread::current->wait_on(m_wait_queue);
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
	return m_children.emplace_back(name);
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
	for (auto& i : m_children) {
		if (strcmp(file_name, i.m_filename) == 0) {
			return i;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}
