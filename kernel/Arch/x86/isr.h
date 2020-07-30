#pragma once

#include "utils/types.h"

#define PF_PRESENT(x) (x & 1)  // The fault was caused by a page-level protection violation.
#define PF_WR(x)      (x & 2)  // The access causing the fault was a write.
#define PF_US(x)      (x & 4)  // A user-mode access caused the fault.
#define PF_RSVD(x)    (x & 8)  // The fault was caused by a reserved bit violation.
#define PF_ID(x)      (x & 16) // The fault was caused by an instruction fetch.
#define PF_PK(x)      (x & 32) // There was a protection-key violation.
#define PF_SGK(x)     (x & 62) // The fault resulted from violation of SGX-specific access-control requirements.

#define SCHEDULE_IRQ 0x81
#define SYSCALL_IRQ  0x80

struct RegistersContext {
	uint32_t ds;
	uint32_t es;
	uint32_t fs;
	uint32_t gs;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
};
struct ISRContextFrame {
	uint32_t context_stack;
	RegistersContext registers;
	uint32_t irq_number;
	uint32_t error_code;
	uint32_t eip, cs, eflags;
	// uint32_t useresp, ss;
};

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

typedef void (*isr_function)(ISRContextFrame*);
extern "C" uintptr_t isr_vector[];

class ISR
{
  private:
	static const char* exception_messages[];

  public:
	static void default_interrupt_handler(ISRContextFrame* info);
	static void initiate_isr_dispatcher_vector();
	static void register_isr_handler(isr_function address, uint8_t irq_number);
};
