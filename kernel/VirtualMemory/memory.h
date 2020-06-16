#pragma once

#include "Arch/x86/isr.h"
#include "utils/types.h"

enum MEMORY_TYPE {
	KERNEL = 1,
	WRITABLE = 2,
	COPY_ON_WRITE = 4,
};

class Memory
{
  private:
	static void page_fault_handler(ContextFrame* isr_info);
	static uint32_t parse_flags(uint32_t mem_flags);
	static uint32_t get_kernel_pages();

  public:
	static void setup();
	static uintptr_t alloc(uint32_t size, uint32_t flags);
	static uintptr_t alloc(uintptr_t virtual_address, uint32_t size, uint32_t flags);
	static uintptr_t free(uintptr_t virtual_address, uint32_t size, uint32_t flags);
	static uintptr_t map(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);
	static void unmap(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);
	static uint32_t virtual_memory_size();
	static uint32_t physical_memory_size();
	static void switch_page_directory(uintptr_t physical_address);
	static void setup_page_fault_handler();
};
