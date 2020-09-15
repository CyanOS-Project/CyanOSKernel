#pragma once

#include "Gdt.h"
#include "Isr.h"
#include <Types.h>

enum class ContextType { Kernel, User };
struct ContextInformation {
	void* stack;
	uint32_t stack_size;
	uint32_t start_function;
	uint32_t return_function;
	uint32_t argument;
};

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
	static uint32_t setup_task_stack_context(ContextType type, ContextInformation& info);
	static void switch_task_stack(uint32_t task_stack_start);
	static void enter_usermode(uintptr_t thread_address, uintptr_t thread_stack);

	static void set_return_value(ISRContextFrame* frame, uint32_t value);
	static void set_return_arg1(ISRContextFrame* frame, uint32_t value);
	static void set_return_arg2(ISRContextFrame* frame, uint32_t value);
	static void set_return_arg3(ISRContextFrame* frame, uint32_t value);

	static uint32_t syscall_num(ISRContextFrame* frame);
	static uint32_t syscall_param1(ISRContextFrame* frame);
	static uint32_t syscall_param2(ISRContextFrame* frame);
	static uint32_t syscall_param3(ISRContextFrame* frame);
	static uint32_t syscall_param4(ISRContextFrame* frame);
	static uint32_t syscall_param5(ISRContextFrame* frame);
};
