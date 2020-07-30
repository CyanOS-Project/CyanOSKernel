#include "SystemCall.h"

void SystemCall::setup()
{
	ISR::register_isr_handler(systemcall_handler, SYSCALL_IRQ);
}

void SystemCall::systemcall_handler(ISRContextFrame* frame)
{
	printf("System Call happened: %x (%x, %x, %x, %x, %x)!\n", syscall_num(frame), syscall_param1(frame),
	       syscall_param2(frame), syscall_param3(frame), syscall_param4(frame), syscall_param5(frame));
}