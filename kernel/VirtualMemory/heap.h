#pragma once
#include "VirtualMemory/memory.h"
#include "kernel_map.h"
#include "types.h"

#define ADDR_TO_HEADER(x) ((unsigned)x - sizeof(BlockHeader))
#define HEADER_TO_ADDR(x) ((unsigned)x + sizeof(BlockHeader))

struct BlockHeader {
	unsigned size;
	BlockHeader *next, *previous;
};

BlockHeader* find_block(unsigned size);
uintptr_t kmalloc(unsigned size, unsigned flags);
void setup_heap();