#pragma once
#include "Arch/x86/context.h"
#include "utils/types.h"

typedef int (*generic_syscall)(int arg0, int arg1, int arg2, int arg3, int arg4);

class SystemCall
{
  private:
	static void systemcall_handler(ISRContextFrame* frame);

  public:
	static generic_syscall systemcalls_routines[];
	static void setup();
};

int OpenFile(char* path, int mode, int flags);
int ReadFile(void* buff, size_t size);
int WriteFile(void* buff, size_t size);
int CloseFile();

int OpenDevice(char* path, int mode, int flags);
int ReadDevice(void* buff, size_t size);
int WriteDevice(void* buff, size_t size);
int CloseDevice();

int CreateThread(void* address, int arg);
int CreateRemoteThread(int process, void* address, int arg);
int CreateProcess(char* path, int flags);
int Sleep(size_t size);
int Yield();