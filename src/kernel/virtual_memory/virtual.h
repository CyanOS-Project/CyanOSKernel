#pragma once

#include "arch/x86/paging.h"
#include "arch/x86/panic.h"
#include "kernel_map.h"
#include "memory.h"
#include <assert.h>
#include <types.h>

class VirtualMemory
{
  public:
	static uintptr_t find_pages(u32 start_address, u32 end_address, u32 pages_num);
	static uintptr_t create_page_table();
	static bool check_free_pages(u32 start_address, u32 pages_num);
	static void map_pages(uintptr_t virtual_address, uintptr_t physical_address, u32 pages, u32 flags);
};
