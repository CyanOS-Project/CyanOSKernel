#pragma once

#include "Arch/x86/Paging.h"
#include "Arch/x86/Panic.h"
#include "Kernel_map.h"
#include <Assert.h>
#include <Clib.h>
#include <Types.h>

#define MAX_PHYSICAL_4K_PAGES (1024 * 1024)
#define TEST_BIT(value, bit)  (((value) >> (bit)) & 1)

class PhysicalMemory
{
  public:
	static void setup();
	static bool check_free_pages(uintptr_t page_number, unsigned count);
	static uintptr_t alloc_page(uintptr_t start);
	static uintptr_t alloc_contagious_pages(uintptr_t start, size_t count);
	static void free_pages(uintptr_t page_number, unsigned count);
	static void set_free_pages(uintptr_t page_number, unsigned count);
	static void set_used_pages(uintptr_t page_number, unsigned count);
	static unsigned get_physical_memory_size();

  private:
	static uintptr_t find_pages(uintptr_t start_page, size_t count);
	static volatile uint8_t physical_memory_tracer[];
	static volatile unsigned physical_memory_size;
};
