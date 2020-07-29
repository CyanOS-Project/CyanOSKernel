#pragma once

#include "isr.h"
#include "utils/types.h"

struct InitialTaskContext {
	ISRContextFrame isr_frame;
	uintptr_t return_address;
	uintptr_t argument;
};

uint32_t setup_task_stack_context(void* stack, uint32_t stack_size, uint32_t start_function, uint32_t return_function,
                                  uint32_t argument);
void switch_task_stack(uint32_t task_stack_start);
void enter_usermode(uintptr_t thread_address, uintptr_t thread_stack);