
#include "isr.h"
#include "console.h"
#include "idt.h"

static isr_function interrupt_dispatcher_vector[NUMBER_OF_IDT_ENTRIES] __attribute__((aligned(4)));
uintptr_t isr_vector[NUMBER_OF_IDT_ENTRIES] __attribute__((aligned(4)));

extern "C" uintptr_t _isr_vector[];
extern "C" uintptr_t _generic_isr;

void initiate_isr_vector()
{
	for (size_t i = 0; i < NUMBER_OF_IDT_DEFINED_ENTRIES; i++) {
		isr_vector[i] = _isr_vector[i];
	}
	for (size_t i = NUMBER_OF_IDT_DEFINED_ENTRIES; i < NUMBER_OF_IDT_ENTRIES; i++) {
		isr_vector[i] = _generic_isr;
	}
	for (size_t i = 0; i < NUMBER_OF_IDT_ENTRIES; i++) {
		interrupt_dispatcher_vector[i] = 0;
	}
}

void add_custom_isr(isr_function address, uint8_t irq_number)
{
	interrupt_dispatcher_vector[irq_number] = (isr_function)address;
}

extern "C" void __attribute__((cdecl)) interrupt_dispatcher(ISR_INFO info)
{
	if (interrupt_dispatcher_vector[info.irq_number]) {
		interrupt_dispatcher_vector[info.irq_number](info);
	} else {
		default_interrupt_handler(info);
	}
}

void default_interrupt_handler(ISR_INFO info)
{
	if (info.irq_number < NUMBER_OF_IDT_ENTRIES) {
		printf("Exception: ");
		printf(exception_messages[info.irq_number]);
		printf("\nContext Dump:\n");
		printf("EIP=%X\t CS=%d\t ESP=%X\t\n", info.eip, info.cs, info.esp);
	} else if (info.irq_number < NUMBER_OF_IDT_EXCEPTIONS) {
		printf("Reserved Exception Number\n");
	} else {
		printf("Undefined IRQ Number\n");
	}
}

const char* exception_messages[19] = {"Division by zero",
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