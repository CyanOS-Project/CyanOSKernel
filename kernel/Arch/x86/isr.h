
#pragma once
#include "types.h"

struct ISR_INFO {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t irq_number;
	uint32_t error_code;
	uint32_t eip, cs, eflags, useresp, ss;
};

typedef void (*isr_function)(ISR_INFO);

void initiate_isr_vector();
void register_isr_handler(isr_function address, uint8_t irq_number);
static void default_interrupt_handler(ISR_INFO info);

extern uintptr_t isr_vector[];
extern const char* exception_messages[];