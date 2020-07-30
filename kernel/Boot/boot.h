#pragma once

#include "Arch/x86/asm.h"
#include "VirtualMemory/memory.h"
#include "kernel_init.h"
#include "kernel_map.h"
#include "multiboot2.h"
#include "utils/assert.h"
#include "utils/types.h"

typedef struct __attribute__((__packed__)) {
	multiboot_header header __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_address address __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_entry_address entry __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_module_align mod_align __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag end __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
} Mutiboot2_Header;
#define MULTIBOOT2_HEADER_CHECKSUM ((uint32_t)(-sizeof(Mutiboot2_Header) - MULTIBOOT2_HEADER_MAGIC))

#define STACK_SIZE 0x4000

extern uint32_t kernel_boot_stage1;
extern "C" void kernel_boot_stage2(uint32_t magic, multiboot_tag_start* boot_info);
BootloaderInfo parse_mbi(uintptr_t multiboot_info);
uintptr_t align_to(uintptr_t size, unsigned alignment);