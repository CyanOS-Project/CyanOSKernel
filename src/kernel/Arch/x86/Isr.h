#pragma once

#include "Asm.h"
#include "Gdt.h"
#include "Idt.h"
#include <Types.h>

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
	u32 gs;
	u32 fs;
	u32 es;
	u32 ds;
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
};
struct ISRContextFrame {
	u32 context_stack;
	RegistersContext registers;
	u32 irq_number;
	u32 error_code;
	u32 eip, cs, eflags;
	// u32 useresp, ss;
};

enum IRQ_Number
{
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

typedef void (*ISRFunction)(ISRContextFrame&);
enum class ISRType
{
	Hardware,
	Software
};
struct ISRHandler {
	ISRFunction function;
	ISRType type;
};

extern "C" uintptr_t isr_vector[];
class ISR
{
  public:
	static void default_interrupt_handler(ISRContextFrame& info);
	static void register_software_interrupt_handler(ISRFunction address, uint8_t IRQ_Number);
	static void register_hardware_interrupt_handler(ISRFunction address, uint8_t interrupt_line);

  private:
	static const char* exception_messages[];
};
