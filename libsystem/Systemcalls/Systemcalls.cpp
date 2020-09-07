#include "Systemcalls.h"

HANDLE OpenFile(const char* path, int mode, int flags)
{
	return syscall3(0, size_t(path), size_t(mode), size_t(flags));
}

size_t ReadFile(HANDLE handle, void* buff, size_t count)
{
	return syscall3(1, size_t(handle), size_t(buff), size_t(count));
}

size_t WriteFile(HANDLE handle, const void* buff, size_t count)
{
	return syscall3(2, size_t(handle), size_t(buff), size_t(count));
}

void CloseFile(HANDLE handle)
{
	syscall1(3, size_t(handle));
}