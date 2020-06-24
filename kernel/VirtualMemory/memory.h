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
	static unsigned get_kernel_pages();

  public:
	static void setup();
	static void* alloc(uint32_t size, uint32_t flags);
	static void* alloc(void* virtual_address, uint32_t size, uint32_t flags);
	static void free(void* virtual_address, uint32_t size, uint32_t flags);
	static void map(void* virtual_address, void* physical_address, uint32_t size, uint32_t flags);
	static void unmap(void* virtual_address, void* physical_address, uint32_t size, uint32_t flags);
	static unsigned virtual_memory_size();
	static unsigned physical_memory_size();
	static void switch_page_directory(uintptr_t physical_address);
	static void setup_page_fault_handler();
};
