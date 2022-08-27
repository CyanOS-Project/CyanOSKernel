#include "context.h"

u32 Context::setup_task_stack_context(ContextType type, ContextInformation& info)
{
	UNUSED(type);

	InitialTaskContext* context =
	    reinterpret_cast<InitialTaskContext*>(uintptr_t(info.stack) + info.stack_size - sizeof(InitialTaskContext));
	context->return_address = info.return_function;
	context->argument = info.argument;
	context->isr_frame.eip = info.start_function;
	context->isr_frame.cs = KCS_SELECTOR;
	context->isr_frame.registers.ds = KDS_SELECTOR;
	context->isr_frame.registers.es = KDS_SELECTOR;
	context->isr_frame.registers.fs = TIB_SELECTOR;
	context->isr_frame.registers.gs = PIB_SELECTOR;

	context->isr_frame.eflags = EFLAGS_IF_ENABLE;
	return u32(&context->isr_frame) + 4;
}

void Context::switch_task_stack(u32 task_stack_start, u32 pib, u32 tib)
{
	GDT::set_tss_stack(task_stack_start);
	GDT::load_gs(pib);
	GDT::load_fs(tib);
}

void Context::enter_usermode(uintptr_t address, uintptr_t stack)
{
	asm volatile("mov %0, %%ds;"
	             "mov %0, %%es;"
	             "pushl %0;"
	             "pushl %1;"
	             "pushl %2;"
	             "pushl %3;"
	             "pushl %4;"
	             "iret;"
	             :
	             : "r"(u32{UDS_SELECTOR}), "r"(u32{stack}), "i"(u32{EFLAGS_IF_ENABLE}), "r"(u32{UCS_SELECTOR}),
	               "r"(u32{address}));
}

void Context::set_return_value(ISRContextFrame& frame, u32 value)
{
	frame.registers.eax = value;
}

void Context::set_return_arg1(ISRContextFrame& frame, u32 value)
{
	frame.registers.ecx = value;
}

void Context::set_return_arg2(ISRContextFrame& frame, u32 value)
{
	frame.registers.edx = value;
}

void Context::set_return_arg3(ISRContextFrame& frame, u32 value)
{
	frame.registers.ebx = value;
}

u32 Context::syscall_num(ISRContextFrame& frame)
{
	return frame.registers.eax;
}

u32 Context::syscall_param1(ISRContextFrame& frame)
{
	return frame.registers.ecx;
}

u32 Context::syscall_param2(ISRContextFrame& frame)
{
	return frame.registers.edx;
}

u32 Context::syscall_param3(ISRContextFrame& frame)
{
	return frame.registers.ebx;
}

u32 Context::syscall_param4(ISRContextFrame& frame)
{
	return frame.registers.esi;
}

u32 Context::syscall_param5(ISRContextFrame& frame)
{
	return frame.registers.edi;
}