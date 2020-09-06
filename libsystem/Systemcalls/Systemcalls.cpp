#include "Systemcalls.h"

int OpenFile(const char* path, int mode, int flags)
{
	return syscall3(0, size_t(path), size_t(mode), size_t(flags));
}

int ReadFile(HANDLE handle, void* buff, size_t count)
{
	return syscall3(1, size_t(handle), size_t(buff), size_t(count));
}

int WriteFile(HANDLE handle, const void* buff, size_t count)
{
	return syscall3(1, size_t(handle), size_t(buff), size_t(count));
}

int CloseFile(HANDLE handle)
{
	return syscall1(1, size_t(handle));
}