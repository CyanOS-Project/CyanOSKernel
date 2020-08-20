#include "SystemCall.h"
#include "Arch/x86/isr.h"
#include "Devices/Console/console.h"

void SystemCall::setup()
{
	ISR::register_isr_handler(systemcall_handler, SYSCALL_IRQ);
}

void SystemCall::systemcall_handler(ISRContextFrame* frame)
{
	printf("System Call happened: %x (%x, %x, %x, %x, %x)!\n", Context::syscall_num(frame),
	       Context::syscall_param1(frame), Context::syscall_param2(frame), Context::syscall_param3(frame),
	       Context::syscall_param4(frame), Context::syscall_param5(frame));
}

generic_syscall SystemCall::systemcalls_routines[] = {reinterpret_cast<generic_syscall>(OpenFile),
                                                      reinterpret_cast<generic_syscall>(ReadFile),
                                                      reinterpret_cast<generic_syscall>(WriteFile),
                                                      reinterpret_cast<generic_syscall>(CloseFile),

                                                      reinterpret_cast<generic_syscall>(OpenDevice),
                                                      reinterpret_cast<generic_syscall>(ReadDevice),
                                                      reinterpret_cast<generic_syscall>(WriteDevice),
                                                      reinterpret_cast<generic_syscall>(CloseDevice),

                                                      reinterpret_cast<generic_syscall>(CreateThread),
                                                      reinterpret_cast<generic_syscall>(CreateRemoteThread), //
                                                      reinterpret_cast<generic_syscall>(CreateProcess),
                                                      reinterpret_cast<generic_syscall>(Sleep),
                                                      reinterpret_cast<generic_syscall>(Yield)};

int OpenFile(char* path, int mode, int flags)
{
	return 0;
}

int ReadFile(void* buff, size_t size)
{
	return 0;
}

int WriteFile(void* buff, size_t size)
{
	return 0;
}

int CloseFile()
{
	return 0;
}

int OpenDevice(char* path, int mode, int flags)
{
	return 0;
}

int ReadDevice(void* buff, size_t size)
{
	return 0;
}

int WriteDevice(void* buff, size_t size)
{
	return 0;
}

int CloseDevice()
{
	return 0;
}

int CreateThread(void* address, int arg)
{
	return 0;
}

int CreateRemoteThread(int process, void* address, int arg)
{
	return 0;
}

int CreateProcess(char* path, int flags)
{
	return 0;
}

int Sleep(size_t size)
{
	return 0;
}

int Yield()
{
	return 0;
}
