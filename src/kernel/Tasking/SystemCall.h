#pragma once
#include "Arch/x86/Context.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Tasking/Process.h"
#include <Result.h>
#include <Types.h>

typedef Result<int> (*generic_syscall)(int arg0, int arg1, int arg2, int arg3, int arg4);

class SystemCall
{
  public:
	static void setup();

  private:
	static generic_syscall systemcalls_routines[];
	static unsigned syscalls_count;

	static generic_syscall get_syscall_routine(unsigned syscall_num);
	static void systemcall_handler(ISRContextFrame& frame);
};

Result<int> OpenFile(const char* path, int mode, int flags);
Result<int> ReadFile(Handle handle, void* buff, size_t size);
Result<int> WriteFile(Handle handle, void* buff, size_t size);
Result<int> QueryDirectory(Handle handle, FileInfo* info);
Result<int> QueryFileInformation(Handle handle, FileInfo* info);
Result<int> CloseHandle(Handle handle);

Result<int> CreateThread(Handle process, void* address, int arg);
Result<int> CreateProcess(const char* path, const char* argument, int flags);
Result<int> OpenProcess(size_t pid, int access);
Result<int> TerminateProcess(Handle handle, int status);
Result<int> TerminateThread(Handle handle, int status);
Result<int> WaitSignal(Handle handle, int signal);
Result<int> Sleep(size_t size);
Result<int> Yield();

Result<int> VirtualAlloc(void* address, size_t size, int flags);
Result<int> VirtualFree(void* address, size_t size);

Result<int> QueryProcessList();
Result<int> QueryThreadList();
Result<int> QuerySystemInformation();
Result<int> QueryProcessInformation();
Result<int> QueryThreadInformation();
