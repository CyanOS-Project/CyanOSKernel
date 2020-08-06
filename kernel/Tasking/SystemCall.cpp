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