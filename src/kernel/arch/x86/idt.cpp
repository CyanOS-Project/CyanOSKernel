#include "idt.h"
#include "pic.h"
volatile IDT_DISCRIPTOR IDT::idt __attribute__((aligned(4)));
volatile IDTEntry IDT::idt_entries[NUMBER_OF_IDT_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

void IDT::setup()
{
	fill_idt((u32)idt_entries, sizeof(IDTEntry) * NUMBER_OF_IDT_ENTRIES);

	for (size_t i = 0; i < NUMBER_OF_IDT_ENTRIES; i++)
		fill_idt_entry(i, (u32)isr_vector[i], KCS_SELECTOR,
		               IDT_ENTRY_FLAGS::PRESENT | IDT_ENTRY_FLAGS::GATE_32 | IDT_ENTRY_FLAGS::INT_GATE |
		                   IDT_ENTRY_FLAGS::RING3);
	load_idt();
	PIC::setup();
}

void IDT::fill_idt(u32 base, u16 limit)
{
	idt.base = base;
	idt.limit = limit;
}

void IDT::fill_idt_entry(uint8_t idt_entry, u32 address, u16 segment, uint8_t type)
{
	idt_entries[idt_entry].offset0_15 = ((u32)address & 0xFFFF);
	idt_entries[idt_entry].offset16_31 = ((u32)address & 0xFFFF0000) >> 16;
	idt_entries[idt_entry].segment = segment;
	idt_entries[idt_entry].type = type;
	idt_entries[idt_entry].zero = 0;
}

void IDT::load_idt()
{
	asm volatile("lidt (%0)" : : "r"(&idt));
}
