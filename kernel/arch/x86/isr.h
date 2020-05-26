
#pragma once
#include "types.h"

struct ISR_INFO {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t irq_number;
	uint32_t error_code;
	uint32_t eip, cs, eflags, useresp, ss;
};

void initiate_isr_vector();
void default_interrupt_handler(ISR_INFO info);

extern uintptr_t interrupt_dispatcher_vector[];
extern const char* exception_messages[];