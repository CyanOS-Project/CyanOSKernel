#include "Pipe.h"

Pipe::Pipe()
{
	// TODO
}

Pipe::~Pipe()
{
	// TODO
}

Result<void> Pipe::read(void* buff, size_t offset, size_t size)
{
	// TODO
}

Result<void> Pipe::write(void* buff, size_t offset, size_t size)
{
	// TODO
}

Result<bool> Pipe::can_read()
{
	// TODO
}

Result<bool> Pipe::can_write()
{
	// TODO
}

Result<void> Pipe::remove()
{
	// TODO
}

Result<void> Pipe::create(char* name, void* info)
{
	// TODO
}

Result<void> Pipe::mkdir(char* name, void* info)
{
	// TODO
}

Result<void> Pipe::link(FSNode& node)
{
	// TODO
}

Result<void> Pipe::unlink(FSNode& node)
{
	// TODO
}

Result<FSNode&> Pipe::dir_lookup(char* file_name)
{
	// TODO
}
