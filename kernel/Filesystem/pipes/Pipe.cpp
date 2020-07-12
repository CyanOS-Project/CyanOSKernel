#include "Pipe.h"

Pipe::Pipe()
{
}

Pipe::~Pipe()
{
}

Result<void> Pipe::read(void* buff, size_t offset, size_t size)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::write(void* buff, size_t offset, size_t size)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
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

Result<void> Pipe::create(char* name, void* info)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::mkdir(char* name, void* info)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::link(FSNode& node)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::unlink(FSNode& node)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> Pipe::dir_lookup(char* file_name)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}
