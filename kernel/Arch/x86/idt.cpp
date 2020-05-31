#include "idt.h"
#include "gdt.h"
#include "isr.h"

volatile IDT idt __attribute__((aligned(4)));
volatile IDTEntry idt_entries[NUMBER_OF_IDT_ENTRIES] __attribute__((aligned(PAGE_4K)));

void testISR(ISR_INFO);

void setup_idt()
{
	initiate_isr_vector();
	fill_idt(&idt, (uint32_t)idt_entries, sizeof(idt_entries) - 1);

	for (size_t i = 0; i < NUMBER_OF_IDT_ENTRIES; i++)
		fill_idt_entry(&idt_entries[i], (uint32_t)isr_vector[i], KCS_SELECTOR,
		               IDT_ENTRY_FLAGS::PRESENT | IDT_ENTRY_FLAGS::GATE_32 | IDT_ENTRY_FLAGS::INT_GATE);
	// register_isr_handler(testISR, EXCEPTION_NUMBER::BP);
	load_idt(&idt);
}

static void fill_idt(volatile IDT* idt, uint32_t base, uint16_t limit)
{
	idt->base = base;
	idt->limit = limit;
}

static void fill_idt_entry(volatile IDTEntry* idt_entry, uint32_t address, uint16_t segment, uint8_t type)
{
	idt_entry->offset0_15 = ((uint32_t)address & 0xFFFF);
	idt_entry->offset16_31 = ((uint32_t)address & 0xFFFF0000) >> 16;
	idt_entry->segment = segment;
	idt_entry->type = type;
	idt_entry->zero = 0;
}

static void load_idt(volatile IDT* idt)
{
	asm volatile("LIDT (%0)" : : "r"(idt));
}

void testISR(ISR_INFO info)
{
	asm("nop");
	return;
}