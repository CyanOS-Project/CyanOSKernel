#include "heap.h"
#include "Arch/x86/paging.h"

volatile PageFrameBlock* malloc_mem;

void setup_heap()
{
	malloc_mem = nullptr;
	malloc_mem = create_new_page();
}
uintptr_t kmalloc(unsigned size, unsigned flags)
{
	if (!size)
		return (uintptr_t) nullptr;

	BlockHeader* free_block;
	// Check if there is a free block in the list
	free_block = find_free_block(size + sizeof(BlockHeader));
	if (free_block) {
		link_block(free_block->previous, free_block);
		free_block->size = size;
		return (uintptr_t)HEADER_TO_ADDR(free_block);
	}
	// create new memory page
	PageFrameBlock* new_page = create_new_page();
	free_block = initiate_first_block(new_page);
	free_block->size = size;
	return (uintptr_t)HEADER_TO_ADDR(free_block);
}

void kfree(uintptr_t addr)
{
	BlockHeader* current_block = (BlockHeader*)ADDR_TO_HEADER(addr);
	unlink_block(current_block);
}

PageFrameBlock* create_new_page()
{
	PageFrameBlock* new_page = (PageFrameBlock*)memory_alloc(MALLOC_PAGE_SIZE, 0);
	new_page->size = MALLOC_PAGE_SIZE;

	PageFrameBlock* last_page = get_last_page();
	if (last_page) {
		last_page->next = new_page;
	}
	return new_page;
}

BlockHeader* initiate_first_block(PageFrameBlock* new_page)
{
	BlockHeader* empty_block = (BlockHeader*)(new_page + 1);
	BlockHeader* free_block = (empty_block + 1);
	empty_block->size = 0;
	empty_block->previous = 0;
	empty_block->next = free_block;
	free_block->previous = empty_block;
	return free_block;
}

void link_block(BlockHeader* current_block, BlockHeader* new_block)
{
	BlockHeader* prev_block = current_block;
	BlockHeader* next_block = current_block->next;
	prev_block->next = new_block;
	if (next_block)
		next_block->previous = new_block;
}

void unlink_block(BlockHeader* current_block)
{
	BlockHeader* prev_block = current_block->previous;
	BlockHeader* next_block = current_block->next;
	prev_block->next = next_block;
	if (next_block) {
		next_block->previous = prev_block;
	}
}

BlockHeader* find_free_block(unsigned size)
{
	volatile PageFrameBlock* current_page = malloc_mem;
	BlockHeader *current_block, *free_block;
	unsigned current_free_space;
	while (current_page) {
		current_block = (BlockHeader*)(current_page + 1);
		while (current_block) {
			if (current_block->next) {
				current_free_space = (unsigned)current_block->next - NEXT_NEIGHBOR_BLOCK(current_block);
			} else { // last block in the current page
				current_free_space = (unsigned)current_page + current_page->size - NEXT_NEIGHBOR_BLOCK(current_block);
			}
			if (current_free_space >= size) {
				free_block = (BlockHeader*)NEXT_NEIGHBOR_BLOCK(current_block);
				free_block->previous = current_block;
				free_block->next = current_block->next;
				return free_block;
			}

			current_block = current_block->next;
		}
		current_page = current_page->next;
	}
	return nullptr;
}

PageFrameBlock* get_last_page()
{
	if (!malloc_mem) {
		return nullptr;
	}
	volatile PageFrameBlock* current = malloc_mem;
	while (malloc_mem->next) {
		current = current->next;
	}
	return (PageFrameBlock*)current;
}