#include "heap.h"
#include "Arch/x86/paging.h"

BlockHeader* malloc_mem;

void setup_heap()
{
	malloc_mem = (BlockHeader*)memory_alloc(PAGE_SIZE, 0);
	malloc_mem->next = 0;
	malloc_mem->size = 0;
}
uintptr_t kmalloc(unsigned size, unsigned flags)
{
	BlockHeader* free_block = find_block(size);
	// Check if there is a free block in the list
	if (free_block) {
		BlockHeader* prev = free_block->previous;
		BlockHeader* next = free_block->next;
		prev->next = free_block;
		next->previous = free_block;
		return (uintptr_t)HEADER_TO_ADDR(free_block);
	}
	// Check if there is an empty space for a new block in the page

	// Create new page
}

BlockHeader* find_block(unsigned size)
{
	BlockHeader* current = malloc_mem;
	BlockHeader* free_block;
	while (current) {
		unsigned current_free_space = (unsigned)current->next - ((unsigned)current->size + (unsigned)current);
		if (current_free_space >= (size + sizeof(BlockHeader))) {
			free_block = (BlockHeader*)(ADDR_TO_HEADER(current) + current->size);
			free_block->previous = current;
			free_block->next = current->next;
			return free_block;
		}
		current = current->next;
	}
	return nullptr;
}