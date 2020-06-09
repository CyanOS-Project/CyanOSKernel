#pragma once

#include "Arch/x86/isr.h"
#include "types.h"

#define GET_PAGES(x, boundary) (x / boundary + ((x % boundary) == 0 ? 0 : 1))

enum MEMORY_TYPE {
	KERNEL = 1,
	WRITABLE = 2,
	COPY_ON_WRITE = 4,
};

class Memory
{
  private:
	static void page_fault_handler(ISR_INFO isr_info);
	static uint32_t parse_flags(uint32_t mem_flags);
	static uint32_t get_kernel_pages();

  public:
	static void setup();
	static uintptr_t alloc(uint32_t size, uint32_t flags);
	static uintptr_t free(uintptr_t address, uint32_t size, uint32_t flags);
	static uintptr_t map(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);
	static void unmap(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);
	static uint32_t virtual_memory_size();
	static uint32_t physical_memory_size();
	static void setup_page_fault_handler();
};
