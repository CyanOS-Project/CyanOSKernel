#include "idt.h"
#include "gdt.h"

IDT idt;
IDTEntry idt_entries[NUMBER_OF_IDT_ENTRIES];

void testISR();

void setup_idt()
{
	int dd = sizeof(IDT);
	int dd2 = sizeof(idt_entries);

	fill_idt(&idt, (uint32_t)idt_entries, sizeof(idt_entries));

	for (size_t i = 0; i < NUMBER_OF_IDT_ENTRIES; i++)
		fill_idt_entry(&idt_entries[i], (uint32_t)testISR, KCS_SELECTOR,
		               IDT_ENTRY_FLAGS::PRESENT | IDT_ENTRY_FLAGS::GATE_32 | IDT_ENTRY_FLAGS::INT_GATE);

	load_idt(&idt);
}

static void fill_idt(IDT* idt, uint32_t base, uint16_t limit)
{
	idt->base = base;
	idt->limit = limit;
}

static void fill_idt_entry(IDTEntry* idt_entry, uint32_t address, uint16_t segment, uint8_t type)
{
	idt_entry->offset0_15 = ((uint32_t)address & 0xFFFF);
	idt_entry->offset16_31 = ((uint32_t)address & 0xFFFF0000) >> 16;
	idt_entry->segment = segment;
	idt_entry->type = type;
	idt_entry->zero = 0;
}

static void load_idt(IDT* idt)
{
	asm volatile("LIDT (%0)" : : "r"(idt));
}

void testISR()
{
	while (1) {
		asm("nop");
	}

	return;
}