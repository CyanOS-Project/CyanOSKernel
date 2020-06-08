#pragma once

#include "kernel_map.h"
#include "types.h"

uintptr_t virtual_find_pages(uint32_t start_page, uint32_t end_page, uint32_t num_pages);