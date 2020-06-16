#pragma once

#include "types.h"

#define PF_PRESENT(x) (x & 1)  // The fault was caused by a page-level protection violation.
#define PF_WR(x)      (x & 2)  // The access causing the fault was a write.
#define PF_US(x)      (x & 3)  // A user-mode access caused the fault.
#define PF_RSVD(x)    (x & 4)  // The fault was caused by a reserved bit violation.
#define PF_ID(x)      (x & 5)  // The fault was caused by an instruction fetch.
#define PF_PK(x)      (x & 6)  // There was a protection-key violation.
#define PF_SGK(x)     (x & 16) // The fault resulted from violation of SGX-specific access-control requirements.

#define SCHEDULE_IRQ 0x81

typedef volatile struct ContextFrame_t {
	uint32_t esp, edi, esi, ebp, __esp, ebx, edx, ecx, eax;
	uint32_t cr2;
	uint32_t irq_number;
	uint32_t error_code;
	uint32_t eip, cs, eflags, useresp, ss;
} ContextFrame;

enum IRQ_NUMBER {
	DE = 0,
	DB = 1,
	NMI = 2,
	BP = 3,
	OF = 4,
	BR = 5,
	UD = 6,
	NM = 7,
	DF = 8,
	TS = 10,
	NP = 11,
	SS = 12,
	GP = 13,
	PF = 14,
	MF = 16,
	AC = 17,
	MC = 18,
	XM = 19,
	VE = 20
};

typedef void (*isr_function)(ContextFrame*);
extern "C" uintptr_t isr_vector[];

class ISR
{
  private:
	static const char* exception_messages[];

  public:
	static void default_interrupt_handler(ContextFrame* info);
	static void initiate_isr_dispatcher_vector();
	static void register_isr_handler(isr_function address, uint8_t irq_number);
};
