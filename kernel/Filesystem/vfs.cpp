#include "vfs.h"

File::File()
{
}

File::~File()
{
}

int File::read(void* buffer, size_t count, size_t* read_count)
{
	PANIC("`read` not implemented!");
}

int File::write(void* buffer, size_t count, size_t* written_count)
{
	PANIC("`write` not implemented!");
}

int File::open(const char* path, unsigned flags, unsigned mode)
{
	PANIC("`open` not implemented!");
}

int File::can_read()
{
	PANIC("`can_read` not implemented!");
}

int File::can_write()
{
	PANIC("`can_write` not implemented!");
}
