#pragma once

#include "types.h"

#define GDT_NUMBER_OF_ENTRIES 8192
#define SEGMENT_SELECTOR(index,prv_level) (index<<3 | prv_level)
#define SEGMENT_INDEX(segment) (segment>>3)
#define KCS_SELECTOR SEGMENT_SELECTOR(1,0)
#define KDS_SELECTOR SEGMENT_SELECTOR(2,0)
#define UCS_SELECTOR SEGMENT_SELECTOR(3,3)
#define UDS_SELECTOR SEGMENT_SELECTOR(4,3)
#define TSS_SELECTOR SEGMENT_SELECTOR(0,0)
#define TSS_SELECTOR SEGMENT_SELECTOR(0,0)

#pragma pack(1)
struct GDT {
	uint16_t limit;
	uint32_t base;
};
struct GDTEntry {
	uint16_t lim0_15;
	uint16_t base0_15;
	uint8_t  base16_23;
	uint8_t  access;
	uint8_t  lim16_19 : 4;
	uint8_t  flags    : 4;
	uint8_t  base24_31;
};
#pragma pack()


extern GDT gdt;
extern GDTEntry gdt_entries[GDT_NUMBER_OF_ENTRIES];

void setup_gdt();

static void fill_gdt(GDT* gdt, uint32_t base, uint16_t limit);
static void fill_gdt_entry(GDTEntry* gdt_entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
static void load_segments(uint16_t code_segment, uint16_t data_segment);
static void load_gdt(GDT* gdt);