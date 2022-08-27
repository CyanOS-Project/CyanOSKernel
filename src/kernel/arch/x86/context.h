#pragma once

#include "gdt.h"
#include "isr.h"
#include <types.h>

enum class ContextType
{
	Kernel,
	User
};
struct ContextInformation {
	void* stack;
	u32 stack_size;
	u32 start_function;
	u32 return_function;
	u32 argument;
};

class Context
{
  public:
	static u32 setup_task_stack_context(ContextType type, ContextInformation& info);
	static void switch_task_stack(u32 task_stack_start, u32 pib, u32 tib);
	static void enter_usermode(uintptr_t thread_address, uintptr_t thread_stack);
	static void set_fs_segment(uintptr_t address);
	static void set_gs_segment(uintptr_t address);

	static void set_return_value(ISRContextFrame& frame, u32 value);
	static void set_return_arg1(ISRContextFrame& frame, u32 value);
	static void set_return_arg2(ISRContextFrame& frame, u32 value);
	static void set_return_arg3(ISRContextFrame& frame, u32 value);

	static u32 syscall_num(ISRContextFrame& frame);
	static u32 syscall_param1(ISRContextFrame& frame);
	static u32 syscall_param2(ISRContextFrame& frame);
	static u32 syscall_param3(ISRContextFrame& frame);
	static u32 syscall_param4(ISRContextFrame& frame);
	static u32 syscall_param5(ISRContextFrame& frame);

  private:
	struct InitialTaskContext {
		ISRContextFrame isr_frame;
		uintptr_t return_address;
		uintptr_t argument;
	};
	static const u32 EFLAGS_IF_ENABLE = 0x202;
};
