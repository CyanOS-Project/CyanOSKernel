#pragma once

#include "Arch/x86/Paging.h"
#include "Arch/x86/Panic.h"
#include "Kernel_map.h"
#include "Memory.h"
#include <Assert.h>
#include <Types.h>

class VirtualMemory
{
  public:
	static uintptr_t find_pages(u32 start_address, u32 end_address, u32 pages_num);
	static uintptr_t create_page_table();
	static bool check_free_pages(u32 start_address, u32 pages_num);
	static void map_pages(uintptr_t virtual_address, uintptr_t physical_address, u32 pages, u32 flags);
};
