#pragma once

#include "types.h"

#define NUMBER_OF_IDT_ENTRIES 255

#define TRAPGATE 0xF
#define INTGATE  0xE
#define TSKGATE  0x5

enum IDT_ENTRY_FLAGS {
	IDT_ENTRY_PRESENT = 0x80,
	IDT_ENTRY_RING3 = 0x60,
	IDT_ENTRY_RING0 = 0x10,
	IDT_ENTRY_STORAGE_SEGMENT = 0x00,
	IDT_ENTRY_TRAPGATE = 0xF,
	IDT_ENTRY_INTGATE = 0xE,
	IDT_ENTRY_TASKGATE = 0x5,
};

#pragma pack(1)
struct IDT {
	uint16_t limit;
	uint32_t base;
};
struct IDTEntry {
	uint16_t offset0_15;
	uint16_t segment;
	uint8_t zero;
	uint8_t type;
	uint16_t offset16_31;
};
#pragma pack()

void setup_idt();
static void fill_idt(IDT* idt, uint32_t base, uint16_t limit);
static void fill_idt_entry(IDTEntry* idt_entry, uint32_t address, uint16_t segment, uint8_t type);
static void load_idt(IDT* idt);

extern IDT idt;
extern IDTEntry idt_entries[];
