#pragma once

#include "../Types.h"

HANDLE OpenFile(const char* path, int mode, int flags);
size_t ReadFile(HANDLE handle, void* buff, size_t size);
size_t WriteFile(HANDLE handle, const void* buff, size_t size);
void CloseFile(HANDLE handle);

inline int syscall0(size_t sysnum)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%0;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum));
	return ret_value;
}

inline int syscall1(size_t sysnum, size_t param1)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%0;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1));
	return ret_value;
}

inline int syscall2(size_t sysnum, size_t param1, size_t param2)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%0;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2));
	return ret_value;
}

inline int syscall3(size_t sysnum, size_t param1, size_t param2, size_t param3)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%0;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2), "b"(param3));
	return ret_value;
}

inline int syscall4(size_t sysnum, size_t param1, size_t param2, size_t param3, size_t param4)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%0;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2), "b"(param3), "S"(param4));
	return ret_value;
}

inline int syscall5(size_t sysnum, size_t param1, size_t param2, size_t param3, size_t param4, size_t param5)
{
	int ret_value = 0, error = 0;
	asm volatile("int $0x80;"
	             "mov %%eax,%0;"
	             "mov %%ecx, %1;"
	             : "=a"(error), "=c"(ret_value)
	             : "a"(sysnum), "c"(param1), "d"(param2), "b"(param3), "S"(param4), "D"(param5));
	return ret_value;
}
