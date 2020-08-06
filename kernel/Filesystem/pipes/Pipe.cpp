#include "Pipe.h"
#include "utils/ErrorCodes.h"
#include "utils/PathParser.h"

Pipe::Pipe(const char* name) :
    m_buffer{BUFFER_SIZE},
    m_reader{BUFFER_SIZE},
    m_writer{BUFFER_SIZE} // FIXME: initial value is BUFFER_SIZE not 0
{
	memcpy(m_filename, name, strlen(name) + 1);
}

Pipe::~Pipe()
{
}

Result<void> Pipe::read(void* buff, size_t offset, size_t size)
{
	UNUSED(offset);
	UNUSED(size);
	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size; i++) {
		m_buffer.queue(_buf[i]);
	}
	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::write(void* buff, size_t offset, size_t size)
{
	UNUSED(offset);
	UNUSED(size);
	// FIXME: check if type is not a folder

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

Result<void> Pipe::create(const char* name, void* info)
{
	UNUSED(name);
	UNUSED(info);
	// m_children.emplace_back(name);
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::mkdir(const char* name, void* info)
{
	UNUSED(name);
	UNUSED(info);
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
