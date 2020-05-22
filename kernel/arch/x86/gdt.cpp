#include "gdt.h"

GDT gdt;
GDTEntry gdt_entries[GDT_NUMBER_OF_ENTRIES];

void setup_gdt()
{
    fill_gdt((uint32_t)&gdt, (uint16_t)sizeof(gdt_entries));

    //Empty Entry
    fill_gdt_entry(gdt_entries, 0, 0, 0, 0);
    //Kernel Segments
    fill_gdt_entry(&gdt_entries[1], 0, 0xFFFFF, 0x9B, 0x0D);
    fill_gdt_entry(&gdt_entries[2], 0, 0xFFFFF, 0x93, 0x0D);
    //User Entries
    fill_gdt_entry(&gdt_entries[3], 0, 0xFFFFF, 0xFF, 0x0D);
    fill_gdt_entry(&gdt_entries[4], 0, 0xFFFFF, 0xF3, 0x0D);

    load_gdt(&gdt, SEGMENT_SELECTOR(1, 0), SEGMENT_SELECTOR(2, 0));

}

static void fill_gdt_entry(GDTEntry* gdt_entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt_entry->base0_15 = base & 0x0000FFFF;
    gdt_entry->base16_23 = (base & 0x00FF0000) >> 16;
    gdt_entry->base24_31 = (base & 0xFF000000) >> 24;
    gdt_entry->lim0_15 = limit & 0xFFFF;
    gdt_entry->lim16_19 = (limit & 0x0F0000) >> 16;
    gdt_entry->access = access;
    gdt_entry->flags = flags;
}

static void fill_gdt(uint32_t base, uint16_t limit)
{
    gdt.base = base;
    gdt.limit = limit;
}

static void load_gdt(GDT* gdt, uint16_t code_segment, uint16_t data_segment)
{
}