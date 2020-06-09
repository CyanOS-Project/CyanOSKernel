#pragma once

#include "kernel_map.h"
#include "types.h"

class VirtualMemory
{
  private:
  public:
	static uintptr_t find_pages(uint32_t start_page, uint32_t end_page, uint32_t num_pages);
};
