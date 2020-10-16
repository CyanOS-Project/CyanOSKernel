#include "Systemcalls.h"

// APIs that use systemcalls

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

void QueryDirectory(Handle handle, FileInfo* info)
{
	syscall2(3, size_t(handle), size_t(info));
}

void CloseHandle(Handle handle)
{
	syscall1(4, size_t(handle));
}

void Sleep(size_t ms)
{
	syscall1(5, size_t(ms));
}

void Yield()
{
	syscall0(6);
}

Handle CreateThread(Handle handle, void* address, int arg)
{
	return syscall3(7, size_t(handle), size_t(address), size_t(arg));
}

Handle CreateProcess(const char* path, const char* argument, int flags)
{
	return syscall3(8, size_t(path), size_t(argument), size_t(flags));
}

Handle OpenProcess(size_t pid, int access)
{
	return syscall2(9, size_t(pid), size_t(access));
}

void TerminateProcess(Handle handle, int status)
{
	syscall2(10, size_t(handle), size_t(status));
}

void TerminateThread(Handle handle, int status)
{
	syscall2(11, size_t(handle), size_t(status));
}

void WaitSignal(Handle handle, int signal)
{
	syscall2(12, size_t(handle), size_t(signal));
}

void* VirtualAlloc(void* address, size_t size, int flags)
{
	return reinterpret_cast<void*>(syscall3(13, size_t(address), size_t(size), size_t(flags)));
}

void VirtualFree(void* address, size_t size)
{
	syscall2(14, size_t(address), size_t(size));
}

void QueryFileInformation(Handle handle, FileInfo* info)
{
	syscall2(15, size_t(handle), size_t(info));
}

// Helper functions

Handle GetCurrentProcess()
{
	return Handle(-1);
}

int GetLastError()
{
	int error;
	asm("movl %%fs:0x4, %0" : "=r"(error));
	return error;
}
