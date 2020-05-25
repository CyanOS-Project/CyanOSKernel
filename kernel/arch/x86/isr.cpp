
#include "isr.h"
#include "idt.h"

uintptr_t interrupt_dispatcher_vector[NUMBER_OF_IDT_ENTRIES];
extern "C" uintptr_t isr_vector[];
extern "C" uintptr_t generic_isr;

void initiate_isr_vector()
{
	for (size_t i = 0; i < NUMBER_OF_IDT_DEFINED_ENTRIES; i++) {
		interrupt_dispatcher_vector[i] = isr_vector[i];
	}
	for (size_t i = NUMBER_OF_IDT_DEFINED_ENTRIES; i < NUMBER_OF_IDT_ENTRIES; i++) {
		interrupt_dispatcher_vector[i] = generic_isr;
	}
}
extern "C" void interrupt_dispatcher(int irq, int error_code)
{
	asm volatile("nop");
}
