#pragma once

#include "kernel_map.h"
#include "utils/types.h"

#define MAX_PHYSICAL_4K_PAGES (1024 * 1024)
#define CHECK_BIT(value, bit) (((value) >> (bit)) & 1)

class PhysicalMemory
{
  private:
	static uintptr_t find_pages(uint32_t count);
	static volatile uint8_t physical_memory_tracer[];
	static volatile unsigned physical_memory_size;

  public:
	static void initialize();
	static bool check_free_pages(uintptr_t page_number, unsigned count);
	static uintptr_t alloc_page();
	static uintptr_t alloc_contagious_pages(unsigned count);
	static void free_pages(uintptr_t page_number, unsigned count);
	static void set_free_pages(uintptr_t page_number, unsigned count);
	static void set_used_pages(uintptr_t page_number, unsigned count);
	static unsigned get_physical_memory_size();
};
