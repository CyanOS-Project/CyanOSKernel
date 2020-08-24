#pragma once

#include "Arch/x86/Paging.h"
#include "Arch/x86/Panic.h"
#include "Kernel_map.h"
#include "Memory.h"
#include "Utils/Assert.h"
#include "Utils/Types.h"

class VirtualMemory
{
  private:
  public:
	static uintptr_t find_pages(uint32_t start_address, uint32_t end_address, uint32_t pages_num);
	static uintptr_t create_page_table();
	static bool check_free_pages(uint32_t start_address, uint32_t pages_num);
	static void map_pages(uintptr_t virtual_address, uintptr_t physical_address, uint32_t pages, uint32_t flags);
};
