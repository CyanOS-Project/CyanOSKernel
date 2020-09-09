#include "Idt.h"

volatile IDT_DISCRIPTOR IDT::idt __attribute__((aligned(4)));
volatile IDTEntry IDT::idt_entries[NUMBER_OF_IDT_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

void IDT::setup()
{
	ISR::initiate_isr_dispatcher_vector();
	fill_idt((uint32_t)idt_entries, sizeof(IDTEntry) * NUMBER_OF_IDT_ENTRIES);

	for (size_t i = 0; i < NUMBER_OF_IDT_ENTRIES; i++)
		fill_idt_entry(i, (uint32_t)isr_vector[i], KCS_SELECTOR,
		               IDT_ENTRY_FLAGS::PRESENT | IDT_ENTRY_FLAGS::GATE_32 | IDT_ENTRY_FLAGS::INT_GATE |
		                   IDT_ENTRY_FLAGS::RING3);
	load_idt();
}

void IDT::fill_idt(uint32_t base, uint16_t limit)
{
	idt.base = base;
	idt.limit = limit;
}

void IDT::fill_idt_entry(uint8_t idt_entry, uint32_t address, uint16_t segment, uint8_t type)
{
	idt_entries[idt_entry].offset0_15 = ((uint32_t)address & 0xFFFF);
	idt_entries[idt_entry].offset16_31 = ((uint32_t)address & 0xFFFF0000) >> 16;
	idt_entries[idt_entry].segment = segment;
	idt_entries[idt_entry].type = type;
	idt_entries[idt_entry].zero = 0;
}

void IDT::load_idt()
{
	asm volatile("LIDT (%0)" : : "r"(&idt));
}
