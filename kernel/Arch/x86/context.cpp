#include "context.h"
#include "gdt.h"

uint32_t setup_task_stack_context(void* stack, uint32_t stack_size, uint32_t start_function, uint32_t return_function,
                                  uint32_t argument)
{
	InitialTaskContext* context =
	    reinterpret_cast<InitialTaskContext*>(uintptr_t(stack) + stack_size - sizeof(InitialTaskContext));
	context->return_address = return_function;
	context->argument = argument;
	context->isr_frame.eip = start_function;
	context->isr_frame.cs = KCS_SELECTOR;
	context->isr_frame.eflags = 0x202;
	return uint32_t(&context->isr_frame) + 4;
}

void switch_task_stack(uint32_t task_stack_start)
{
	GDT::set_tss_stack(task_stack_start);
}

void enter_usermode(uintptr_t address, uintptr_t stack)
{
	asm("CLI");

	asm volatile(" MOVW %0,%%ds   \t\n\
				   MOVW %0,%%es   \t\n\
				   MOVW %0,%%fs   \t\n\
				   MOVW %0,%%gs	  \t\n\
				   PUSHL %0			\t\n\
				   PUSHL %1			\t\n\
				   PUSHF			\t\n\
				   PUSHL %2		     \t\n\
				   PUSHL %3   	  \t\n\
				   IRET"
	             :
	             : "r"(uint16_t{UDS_SELECTOR}), "r"(uint32_t{stack}), "r"(uint32_t{UCS_SELECTOR}),
	               "r"(uint32_t{address}));
}