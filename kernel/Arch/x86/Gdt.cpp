#include "Gdt.h"

volatile GDT_DESCRIPTOR GDT::gdt __attribute__((aligned(8)));
volatile GDT_ENTRY GDT::gdt_entries[GDT_NUMBER_OF_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
volatile TSS_ENTRY GDT::tss_entry __attribute__((aligned(PAGE_SIZE)));
void GDT::setup()
{
	memset((void*)&tss_entry, 0, sizeof(TSS_ENTRY));
	memset((void*)&gdt_entries, 0, sizeof(GDT_ENTRY) * GDT_NUMBER_OF_ENTRIES);

	fill_gdt((uint32_t)&gdt_entries, GDT_NUMBER_OF_ENTRIES * sizeof(GDT_ENTRY) - 1);

	// Empty Entry
	fill_gdt_entry(0, 0, 0, 0, 0);
	// Kernel Segments
	fill_gdt_entry(SEGMENT_INDEX(KCS_SELECTOR), 0, 0xFFFFF, GDT_CODE_PL0, 0x0D);
	fill_gdt_entry(SEGMENT_INDEX(KDS_SELECTOR), 0, 0xFFFFF, GDT_DATA_PL0, 0x0D);
	// User Entries
	fill_gdt_entry(SEGMENT_INDEX(UCS_SELECTOR), 0, 0xFFFFF, GDT_CODE_PL3, 0x0D);
	fill_gdt_entry(SEGMENT_INDEX(UDS_SELECTOR), 0, 0xFFFFF, GDT_DATA_PL3, 0x0D);
	// TSS
	fill_gdt_entry(SEGMENT_INDEX(TSS_SELECTOR), uint32_t(&tss_entry), sizeof(TSS_ENTRY), GDT_TSS_PL3, 0);
	// FS and GS
	fill_gdt_entry(SEGMENT_INDEX(TIB_SELECTOR), 0, 0xFFFFF, GDT_DATA_PL3, 0x0D);
	fill_gdt_entry(SEGMENT_INDEX(PIB_SELECTOR), 0, 0xFFFFF, GDT_DATA_PL3, 0x0D);

	load_gdt();

	load_segments(KCS_SELECTOR, KDS_SELECTOR, TIB_SELECTOR, PIB_SELECTOR);
	setup_tss(0);
}

void GDT::set_tss_stack(uint32_t kernel_stack)
{
	memset((void*)&tss_entry, 0, sizeof(TSS_ENTRY));
	tss_entry.esp0 = kernel_stack;
	tss_entry.ss0 = KDS_SELECTOR;
}

void GDT::setup_tss(uint32_t kernel_stack)
{
	set_tss_stack(kernel_stack);
	load_tss(TSS_SELECTOR);
}

void GDT::fill_gdt_entry(uint32_t gdt_entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt_entries[gdt_entry].base0_15 = base & 0x0000FFFF;
	gdt_entries[gdt_entry].base16_23 = (base & 0x00FF0000) >> 16;
	gdt_entries[gdt_entry].base24_31 = (base & 0xFF000000) >> 24;
	gdt_entries[gdt_entry].lim0_15 = limit & 0xFFFF;
	gdt_entries[gdt_entry].lim16_19 = (limit & 0x0F0000) >> 16;
	gdt_entries[gdt_entry].access = access;
	gdt_entries[gdt_entry].flags = flags;
}

void GDT::fill_gdt(uint32_t base, uint16_t limit)
{
	gdt.base = base;
	gdt.limit = limit;
}

void GDT::load_gdt()
{
	asm volatile("lgdt (%0)" : : "r"(&gdt));
}

void GDT::load_fs(uint32_t address)
{
	fill_gdt_entry(SEGMENT_INDEX(TIB_SELECTOR), address, 0xFFFFF, GDT_DATA_PL3, 0x0D);
	load_gdt();
}

void GDT::load_gs(uint32_t address)
{
	fill_gdt_entry(SEGMENT_INDEX(PIB_SELECTOR), address, 0xFFFFF, GDT_DATA_PL3, 0x0D);
	load_gdt();
}

void GDT::load_tss(uint16_t tss)
{
	asm volatile("ltr %0" : : "a"(tss));
}
