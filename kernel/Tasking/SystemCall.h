#pragma once
#include "Arch/x86/Context.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Result.h"
#include "Tasking/Process.h"
#include "Types.h"

typedef Result<int> (*generic_syscall)(int arg0, int arg1, int arg2, int arg3, int arg4);

class SystemCall
{
  private:
	static generic_syscall systemcalls_routines[];
	static unsigned syscalls_count;

	static generic_syscall get_syscall_routine(unsigned syscall_num);
	static void systemcall_handler(ISRContextFrame* frame);

  public:
	static void setup();
};

Result<int> OpenFile(char* path, int mode, int flags);
Result<int> ReadFile(Handle handle, void* buff, size_t size);
Result<int> WriteFile(Handle handle, void* buff, size_t size);
Result<int> CloseFile(Handle handle);
Result<int> QueryDirectory(Handle handle, DirectoryInfo* info);

Result<int> CreateThread(void* address, int arg);
Result<int> CreateRemoteThread(int process, void* address, int arg);
Result<int> CreateProcess(char* name, char* path, int flags);
Result<int> Sleep(size_t size);
Result<int> Yield();