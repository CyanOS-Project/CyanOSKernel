#pragma once

#include "Arch/x86/spinlock.h"
#include "Lib/stdlib.h"
#include "VirtualMemory/memory.h"
#include "utils/types.h"

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
class Heap
{
  private:
	static StaticSpinlock lock;
	static PageFrameBlock* create_new_page();
	static PageFrameBlock* get_last_page();
	static BlockHeader* initiate_first_block(PageFrameBlock* new_page);
	static BlockHeader* find_free_block(unsigned size);
	static void link_block(BlockHeader* current_block, BlockHeader* new_block);
	static void unlink_block(BlockHeader* current_block);
	static volatile PageFrameBlock* malloc_mem;

  public:
	static void setup();
	static void* kmalloc(unsigned size, unsigned flags);
	static void kfree(void* addr);
};
