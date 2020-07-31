#include "context.h"

uint32_t Context::setup_task_stack_context(void* stack, uint32_t stack_size, uint32_t start_function,
                                           uint32_t return_function, uint32_t argument)
{
	InitialTaskContext* context =
	    reinterpret_cast<InitialTaskContext*>(uintptr_t(stack) + stack_size - sizeof(InitialTaskContext));
	context->return_address = return_function;
	context->argument = argument;
	context->isr_frame.eip = start_function;
	context->isr_frame.cs = KCS_SELECTOR;
	context->isr_frame.eflags = EFLAGS_IF_ENABLE;
	return uint32_t(&context->isr_frame) + 4;
}

void Context::switch_task_stack(uint32_t task_stack_start)
{
	GDT::set_tss_stack(task_stack_start);
}

void Context::enter_usermode(uintptr_t address, uintptr_t stack)
{
	asm volatile("MOV %%ds,%0;"
	             "MOV %%es,%0;"
	             "MOV %%fs,%0;"
	             "MOV %%gs,%0;"
	             "PUSH %0;"
	             "PUSH %1;"
	             "PUSH %2;"
	             "PUSH %3;"
	             "PUSH %4;"
	             "IRET;"
	             :
	             : "r"(uint16_t{UDS_SELECTOR}), "r"(uint32_t{stack}), "i"(uint32_t{EFLAGS_IF_ENABLE}),
	               "r"(uint32_t{UCS_SELECTOR}), "r"(uint32_t{address}));
}