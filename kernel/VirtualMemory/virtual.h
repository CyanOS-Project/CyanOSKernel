#pragma once

#include "kernel_map.h"
#include "types.h"

class VirtualMemory
{
  private:
  public:
	static uintptr_t find_pages(uint32_t start_address, uint32_t end_address, uint32_t pages_num);
	static bool check_free_pages(uint32_t start_address, uint32_t pages_num);
};
