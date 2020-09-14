#include "Systemcalls.h"

Handle OpenFile(const char* path, int mode, int flags)
{
	return syscall3(0, size_t(path), size_t(mode), size_t(flags));
}

size_t ReadFile(Handle handle, void* buff, size_t count)
{
	return syscall3(1, size_t(handle), size_t(buff), size_t(count));
}

size_t WriteFile(Handle handle, const void* buff, size_t count)
{
	return syscall3(2, size_t(handle), size_t(buff), size_t(count));
}

void QueryDirectory(Handle handle, void* info)
{
	syscall2(3, size_t(handle), size_t(info));
}

void CloseFile(Handle handle)
{
	syscall1(4, size_t(handle));
}

Handle CreateThread(void* address, int arg)
{
	return syscall2(5, size_t(address), size_t(arg));
}

Handle CreateRemoteThread(Handle handle, void* address, int arg)
{
	return syscall3(6, size_t(handle), size_t(address), size_t(arg));
}

Handle CreateProcess(const char* name, const char* path, int flags)
{
	return syscall3(7, size_t(name), size_t(path), size_t(flags));
}

Handle OpenProcess(size_t pid, int access)
{
	return syscall2(8, size_t(pid), size_t(access));
}

void TerminateProcess(Handle handle, int status)
{
	syscall2(9, size_t(handle), size_t(status));
}

void WaitSignal(Handle handle, int signal)
{
	syscall2(10, size_t(handle), size_t(signal));
}

void Sleep(size_t ms)
{
	syscall1(11, size_t(ms));
}

void Yield()
{
	syscall0(12);
}