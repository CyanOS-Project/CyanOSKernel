
#include "isr.h"
#include "Devices/Console/console.h"
#include "asm.h"
#include "gdt.h"
#include "idt.h"

static isr_function interrupt_dispatcher_vector[NUMBER_OF_IDT_ENTRIES] __attribute__((aligned(4)));

extern "C" uintptr_t isr_vector[];

void ISR::initiate_isr_dispatcher_vector()
{
	for (size_t i = 0; i < NUMBER_OF_IDT_ENTRIES; i++) {
		interrupt_dispatcher_vector[i] = 0;
	}
}

void ISR::register_isr_handler(isr_function address, uint8_t irq_number)
{
	interrupt_dispatcher_vector[irq_number] = (isr_function)address;
}

extern "C" uint32_t __attribute__((cdecl)) interrupt_dispatcher(ContextFrame info)
{
	asm("MOVL %%CR2,%0" : "=r"(info.cr2));
	if (interrupt_dispatcher_vector[info.irq_number]) {
		interrupt_dispatcher_vector[info.irq_number](&info);
	} else {
		ISR::default_interrupt_handler(&info);
	}
	return info.esp;
}

void ISR::default_interrupt_handler(ContextFrame* info)
{
	if (info->irq_number < NUMBER_OF_IDT_ENTRIES) {
		printf("Exception: ");
		printf(exception_messages[info->irq_number]);
		printf("\nContext Dump:\n");
		printf("EIP=%X\t CS=%X\t ESP=%X\t\n", info->eip, info->cs, info->esp);
	} else if (info->irq_number < NUMBER_OF_IDT_EXCEPTIONS) {
		printf("Reserved Exception Number\n");
	} else {
		printf("Undefined IRQ Number (IRQ%d)\n", info->irq_number);
	}
	HLT();
}

const char* ISR::exception_messages[19] = {"Division by zero",
                                           "Debug",
                                           "Non-maskable interrupt",
                                           "Breakpoint",
                                           "Detected overflow",
                                           "Out-of-bounds",
                                           "Invalid opcode",
                                           "No coprocessor",
                                           "Double fault",
                                           "Coprocessor segment overrun",
                                           "Bad TSS",
                                           "Segment not present",
                                           "Stack fault",
                                           "General protection fault",
                                           "Page fault",
                                           "Unknown interrupt",
                                           "Coprocessor fault",
                                           "Alignment check",
                                           "Machine check"};