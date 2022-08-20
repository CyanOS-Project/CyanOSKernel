#pragma once

#include "Arch/x86/Asm.h"
#include "Kernel_init.h"
#include "Kernel_map.h"
#include "Multiboot2.h"
#include "VirtualMemory/Memory.h"
#include <Assert.h>
#include <Types.h>

typedef struct __attribute__((__packed__)) {
	multiboot_header header __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_address address __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_entry_address entry __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_module_align mod_align __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag end __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
} Mutiboot2_Header;
#define MULTIBOOT2_HEADER_CHECKSUM ((u32)(-sizeof(Mutiboot2_Header) - MULTIBOOT2_HEADER_MAGIC))

#define STACK_SIZE 0x4000

extern "C" u32 _kernel_boot_stage1;
extern "C" void kernel_boot_stage2(u32 magic, multiboot_tag_start* boot_info);
Module get_ramdisk(multiboot_tag_start* multiboot_info);