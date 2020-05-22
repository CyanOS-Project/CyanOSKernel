#pragma once

#include "types.h"

#define GDT_NUMBER_OF_ENTRIES 8192
#define SEGMENT_SELECTOR(index,prv_level) (index<<3 | prv_level)

struct __attribute__((__packed__)) GDT {
	uint16_t limit;
	uint32_t base;
};
struct __attribute__((__packed__)) GDTEntry {
	uint16_t lim0_15;
	uint16_t base0_15;
	uint8_t  base16_23;
	uint8_t  access;
	uint8_t  lim16_19 : 4;
	uint8_t  flags    : 4;
	uint8_t  base24_31;
};


extern GDT gdt;
extern GDTEntry gdt_entries[GDT_NUMBER_OF_ENTRIES];

void setup_gdt();
static void fill_gdt(uint32_t base, uint16_t limit);
static void fill_gdt_entry(GDTEntry* gdt_entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
static void load_gdt(GDT* gdt, uint16_t code_segment, uint16_t data_segment);