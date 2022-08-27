#pragma once

#include "kernel_map.h"
#include "paging.h"
#include <clib.h>
#include <types.h>

#define GDT_NUMBER_OF_ENTRIES              8
#define SEGMENT_SELECTOR(index, prv_level) (index << 3 | prv_level)
#define SEGMENT_INDEX(segment)             (segment >> 3)
#define KCS_SELECTOR                       SEGMENT_SELECTOR(1, 0)
#define KDS_SELECTOR                       SEGMENT_SELECTOR(2, 0)
#define UCS_SELECTOR                       SEGMENT_SELECTOR(3, 3)
#define UDS_SELECTOR                       SEGMENT_SELECTOR(4, 3)
#define TSS_SELECTOR                       SEGMENT_SELECTOR(5, 3)
#define TIB_SELECTOR                       SEGMENT_SELECTOR(6, 3) // Thread Information Block
#define PIB_SELECTOR                       SEGMENT_SELECTOR(7, 3) // Process Information Block

#define SEG_DESCTYPE(x) (((x)&0x01) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)     (((x)&0x01) << 0x07) // Present
#define SEG_PRIV(x)     (((x)&0x03) << 0x05) // Set privilege level (0 - 3)

#define SEG_SAVL(x) ((x) << 0x0C) // Available for system use
#define SEG_LONG(x) ((x) << 0x0D) // Long mode
#define SEG_SIZE(x) ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x) ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)

#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed
#define SEG_TSS_AVAILABLE  0x9

#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(0) | SEG_CODE_EXRDA
#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(0) | SEG_DATA_RDWR
#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(3) | SEG_CODE_EXRDA
#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(3) | SEG_DATA_RDWR
#define GDT_TSS_PL3  SEG_DESCTYPE(0) | SEG_PRES(1) | SEG_PRIV(3) | SEG_TSS_AVAILABLE

#pragma pack(1)
struct GDT_DESCRIPTOR {
	u16 limit;
	u32 base;
};
// FIXME: use normal bitmask instead of bit fields.
struct GDT_ENTRY {
	u16 lim0_15;
	u16 base0_15;
	uint8_t base16_23;
	uint8_t access;
	uint8_t lim16_19 : 4;
	uint8_t flags : 4;
	uint8_t base24_31;
};
struct TSS_ENTRY {
	u32 prev_tss;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iomap_base;
};
#pragma pack()

extern "C" void load_segments(u16 cs, u16 ds, u16 fs, u16 gs);

class GDT
{
  public:
	static void setup();
	static void setup_tss(u32 kernel_stack);
	static void set_tss_stack(u32 kernel_stack);
	static void load_fs(u32 address);
	static void load_gs(u32 address);

  private:
	static void fill_gdt(u32 base, u16 limit);
	static void load_gdt();
	static void fill_gdt_entry(u32 gdt_entry, u32 base, u32 limit, uint8_t access, uint8_t flags);
	static void load_tss(u16 tss);

	static volatile GDT_DESCRIPTOR gdt;
	static volatile GDT_ENTRY gdt_entries[];
	static volatile TSS_ENTRY tss_entry;
};
