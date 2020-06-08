#pragma once

#include "VirtualMemory/memory.h"
#include "types.h"

#define ADDR_TO_HEADER(x)                  ((unsigned)x - sizeof(BlockHeader))
#define HEADER_TO_ADDR(x)                  ((unsigned)x + sizeof(BlockHeader))
#define NEXT_NEIGHBOR_BLOCK(current_block) ((unsigned)current_block + sizeof(BlockHeader) + current_block->size)
#define MALLOC_PAGE_SIZE                   PAGE_SIZE
#define MAX_SIZE                           (MALLOC_PAGE_SIZE - sizeof(BlockHeader) * 2 - sizeof(PageFrameBlock))

struct BlockHeader {
	unsigned size;
	BlockHeader *next, *previous;
};

struct PageFrameBlock {
	unsigned size;
	PageFrameBlock *next, *previous;
};

void setup_heap();
uintptr_t kmalloc(unsigned size, unsigned flags);
void kfree(uintptr_t addr);
PageFrameBlock* create_new_page();
BlockHeader* initiate_first_block(PageFrameBlock* new_page);
void link_block(BlockHeader* current_block, BlockHeader* new_block);
void unlink_block(BlockHeader* current_block);
BlockHeader* find_free_block(unsigned size);
PageFrameBlock* get_last_page();
