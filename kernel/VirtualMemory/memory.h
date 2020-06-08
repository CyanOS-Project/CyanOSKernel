#pragma once

#include "Arch/x86/isr.h"
#include "types.h"

#define GET_PAGES(x, boundary) (x / boundary + ((x % boundary) == 0 ? 0 : 1))

enum MEMORY {
	KERNEL = 1,
	WRITABLE = 2,
	COPY_ON_WRITE = 4,
};
void setup_virtual_memory();
uintptr_t memory_alloc(uint32_t size, uint32_t flags);
uintptr_t memory_free(uintptr_t address, uint32_t size, uint32_t flags);
uintptr_t memory_map(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);
void memory_unmap(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);
uint32_t virtual_memory_size();
uint32_t physical_memory_size();
void setup_page_fault_handler();
void page_fault_handler(ISR_INFO isr_info);

static uint32_t get_kernel_pages();
