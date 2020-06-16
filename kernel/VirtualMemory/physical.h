#pragma once

#include "kernel_map.h"
#include "utils/types.h"

#define MAX_PHYSICAL_4K_PAGES (1024 * 1024)
#define CHECK_BIT(value, bit) ((value >> bit) & 1)

class PhysicalMemory
{
  private:
	static uint32_t find_pages(uint32_t count);
	static volatile uint8_t physical_memory_tracer[];
	static volatile uint32_t physical_memory_size;

  public:
	static void initialize();
	static uint32_t alloc_page();
	static uint32_t alloc_contagious_pages(int count);
	static void free_pages(uint32_t page_number, uint32_t count);
	static void set_free_pages(uint32_t page_number, uint32_t count);
	static void set_used_pages(uint32_t page_number, uint32_t count);
	static uint32_t get_physical_memory_size();
};
