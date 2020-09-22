#pragma once

#include "../Types.h"

Handle OpenFile(const char* path, int mode, int flags);
size_t ReadFile(Handle handle, void* buff, size_t size);
size_t WriteFile(Handle handle, const void* buff, size_t size);
void QueryDirectory(Handle handle, void* info);
void CloseHandle(Handle handle);
void Sleep(size_t size);
void Yield();
Handle CreateThread(Handle handle, void* address, int arg);
Handle CreateProcess(const char* path, const char* argument, int flags);
Handle OpenProcess(size_t pid, int access);
void TerminateProcess(Handle handle, int status);
void TerminateThread(Handle handle, int status);
void WaitSignal(Handle handle, int signal);
void* VirtualAlloc(void* address, size_t size, int flags);
void VirtualFree(void* address, size_t size);

int GetLastError();

inline int syscall0(size_t sysnum)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%%fs:0x4;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum));
	// asm("movl %0,%%fs:0x4" : "=r"(error));
	return ret_value;
}

inline int syscall1(size_t sysnum, size_t param1)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%%fs:0x4;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1));
	// asm("movl %0,%%fs:0x4" : "=r"(error));
	return ret_value;
}

inline int syscall2(size_t sysnum, size_t param1, size_t param2)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%%fs:0x4;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2));
	// asm("movl %0,%%fs:0x4" : "=r"(error));
	return ret_value;
}

inline int syscall3(size_t sysnum, size_t param1, size_t param2, size_t param3)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%%fs:0x4;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2), "b"(param3));
	// asm("movl %0,%%fs:0x4" : "=r"(error));
	return ret_value;
}

inline int syscall4(size_t sysnum, size_t param1, size_t param2, size_t param3, size_t param4)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%%fs:0x4;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2), "b"(param3), "S"(param4));
	// asm("movl %0,%%fs:0x4" : "=r"(error));
	return ret_value;
}

inline int syscall5(size_t sysnum, size_t param1, size_t param2, size_t param3, size_t param4, size_t param5)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%%fs:0x4;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2), "b"(param3), "S"(param4), "D"(param5));

	return ret_value;
}
