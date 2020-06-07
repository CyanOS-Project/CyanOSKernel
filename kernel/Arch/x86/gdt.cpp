#include "gdt.h"

volatile GDT_DESCRIPTOR gdt __attribute__((aligned(8)));
volatile GDT_ENTRY gdt_entries[GDT_NUMBER_OF_ENTRIES] __attribute__((aligned(PAGE_4K)));

void GDT::setup()
{
	fill_gdt(&gdt, (uint32_t)&gdt_entries, 5 * sizeof(GDT_ENTRY));

	// Empty Entry
	fill_gdt_entry(gdt_entries, 0, 0, 0, 0);
	// Kernel Segments
	fill_gdt_entry(&gdt_entries[SEGMENT_INDEX(KCS_SELECTOR)], 0, 0xFFFFF, GDT_CODE_PL0, 0x0D);
	fill_gdt_entry(&gdt_entries[SEGMENT_INDEX(KDS_SELECTOR)], 0, 0xFFFFF, GDT_DATA_PL0, 0x0D);
	// User Entries
	fill_gdt_entry(&gdt_entries[SEGMENT_INDEX(UCS_SELECTOR)], 0, 0xFFFFF, GDT_CODE_PL3, 0x0D);
	fill_gdt_entry(&gdt_entries[SEGMENT_INDEX(UDS_SELECTOR)], 0, 0xFFFFF, GDT_DATA_PL3, 0x0D);

	load_gdt(&gdt);
	load_segments(KCS_SELECTOR, KDS_SELECTOR);
}

void GDT::fill_gdt_entry(volatile GDT_ENTRY* gdt_entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt_entry->base0_15 = base & 0x0000FFFF;
	gdt_entry->base16_23 = (base & 0x00FF0000) >> 16;
	gdt_entry->base24_31 = (base & 0xFF000000) >> 24;
	gdt_entry->lim0_15 = limit & 0xFFFF;
	gdt_entry->lim16_19 = (limit & 0x0F0000) >> 16;
	gdt_entry->access = access;
	gdt_entry->flags = flags;
}

void GDT::fill_gdt(volatile GDT_DESCRIPTOR* gdt, uint32_t base, uint16_t limit)
{
	gdt->base = base;
	gdt->limit = limit;
}

void GDT::load_gdt(volatile GDT_DESCRIPTOR* gdt)
{
	asm volatile("LGDT (%0)" : : "r"(gdt));
}

void GDT::load_segments(uint16_t code_segment, uint16_t data_segment)
{
	asm volatile(" MOVW %0,%%ds   \t\n\
				   MOVW %0,%%es   \t\n\
				   MOVW %0,%%ss   \t\n\
				   MOVW %0,%%fs   \t\n\
				   MOVW %0,%%gs"
	             :
	             : "r"(data_segment));
	asm volatile("PUSHL %0\n      \t\n\
				  PUSHL $farjmp   \t\n\
				  RETF            \t\n\
				  farjmp:         \t\n\
				  nop"
	             :
	             : "r"((uint32_t)code_segment));
}
