#pragma once

#include "isr.h"
#include "utils/types.h"
#include "gdt.h"


class Context
{
  private:
	struct InitialTaskContext {
		ISRContextFrame isr_frame;
		uintptr_t return_address;
		uintptr_t argument;
	};
	static const uint32_t EFLAGS_IF_ENABLE = 0x202;

  public:
	static uint32_t setup_task_stack_context(void* stack, uint32_t stack_size, uint32_t start_function,
	                                         uint32_t return_function, uint32_t argument);
	static void switch_task_stack(uint32_t task_stack_start);
	static void enter_usermode(uintptr_t thread_address, uintptr_t thread_stack);

	static inline uint32_t syscall_num(ISRContextFrame* frame)
	{
		return frame->registers.eax;
	}

	static inline uint32_t syscall_param1(ISRContextFrame* frame)
	{
		return frame->registers.ecx;
	}
	static inline uint32_t syscall_param2(ISRContextFrame* frame)
	{
		return frame->registers.edx;
	}
	static inline uint32_t syscall_param3(ISRContextFrame* frame)
	{
		return frame->registers.ebx;
	}
	static inline uint32_t syscall_param4(ISRContextFrame* frame)
	{
		return frame->registers.esi;
	}
	static inline uint32_t syscall_param5(ISRContextFrame* frame)
	{
		return frame->registers.edi;
	}
};
