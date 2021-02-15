#pragma once
#include "../Systemcalls/Systemcalls.h"
#include <Bitmap.h>
#include <Types.h>

const static u32 MAGIC = 0x8CBEEFC8;
const static u32 PAGE_SIZE = 0x1000;

template <size_t size = 1> struct MemoryBlock {
	void* slab_ptr;
	size_t slab_size;
	char data[size];
};

template <size_t slab_size, size_t memory_size> class Slab
{
  private:
	template <size_t max_blocks> struct SlabHeader {
		u32 magic;
		u32 size;
		size_t free_blocks;
		size_t next_fit_block;
		Slab<slab_size, memory_size>*prev, *next;
		Bitmap<max_blocks> mem_map;
	};
	const static size_t MAX_HEADER_SIZE = sizeof(SlabHeader<memory_size / slab_size>);
	const static size_t MAX_BLOCKS = (memory_size - MAX_HEADER_SIZE) / sizeof(MemoryBlock<slab_size>);

	static_assert(memory_size > MAX_HEADER_SIZE);
	static_assert((sizeof(MemoryBlock<slab_size>) + MAX_HEADER_SIZE) <= memory_size);

	bool is_address_in_slab(void* address);
	void* alloc_in_current_slab(size_t block_index);
	void* alloc_in_new_slab();
	void free_from_current_slab(size_t block_index);

  public:
	SlabHeader<MAX_BLOCKS> header;
	MemoryBlock<slab_size> blocks[MAX_BLOCKS];

	void init(Slab* prev = nullptr);
	void* alloc();
	void free(void* address);
};

inline void* request_memory_from_os(size_t size)
{
	void* address = VirtualAlloc(0, size, PAGE_READWRITE);
	return address;
}

inline void free_memory_to_os(void* addrss, size_t size)
{
	VirtualFree(addrss, size);
}

template <size_t slab_size, size_t memory_size> void Slab<slab_size, memory_size>::init(Slab* prev)
{
	header.magic = MAGIC;
	header.size = slab_size;
	header.prev = prev;
	header.next = nullptr;
	header.free_blocks = MAX_BLOCKS;
	header.next_fit_block = 0;
}

template <size_t slab_size, size_t memory_size> void* Slab<slab_size, memory_size>::alloc()
{
	ASSERT(header.magic == MAGIC);
	ASSERT(header.size == slab_size);

	size_t block_index = -1;
	if (header.free_blocks &&
	    ((block_index = header.mem_map.find_first_clear(header.next_fit_block)) != BITMAP_NO_BITS_LEFT)) {
		return alloc_in_current_slab(block_index);
	} else if (header.next) {
		return header.next->alloc();
	} else {
		return alloc_in_new_slab();
	}
}

template <size_t slab_size, size_t memory_size> void Slab<slab_size, memory_size>::free(void* address)
{
	ASSERT(header.magic == MAGIC);
	ASSERT(header.size == slab_size);
	ASSERT(is_address_in_slab(address));

	size_t block_index = (uintptr_t(address) - uintptr_t(blocks)) / sizeof(MemoryBlock<slab_size>);
	ASSERT(header.mem_map.check_set(block_index));
	free_from_current_slab(block_index);
}

template <size_t slab_size, size_t memory_size> bool Slab<slab_size, memory_size>::is_address_in_slab(void* address)
{
	if ((address >= blocks) && (address <= &blocks[MAX_BLOCKS - 1].data[slab_size - 1])) {
		return true;
	} else {
		return false;
	}
	return true;
}

template <size_t slab_size, size_t memory_size> void* Slab<slab_size, memory_size>::alloc_in_new_slab()
{
	Slab* new_slab = static_cast<Slab*>(request_memory_from_os(sizeof(Slab)));
	if (!new_slab) {
		return nullptr;
	}
	new_slab->init(this);
	header.next = new_slab;
	return new_slab->alloc();
}

template <size_t slab_size, size_t memory_size>
void* Slab<slab_size, memory_size>::alloc_in_current_slab(size_t block_index)
{
	header.mem_map.set(block_index);
	header.next_fit_block = (block_index + 1) % MAX_BLOCKS;
	header.free_blocks--;
	blocks[block_index].slab_ptr = this;
	blocks[block_index].slab_size = slab_size;
	return static_cast<void*>(blocks[block_index].data);
}

template <size_t slab_size, size_t memory_size>
void Slab<slab_size, memory_size>::free_from_current_slab(size_t block_index)
{
	header.mem_map.clear(block_index);
	header.next_fit_block = block_index;
	header.free_blocks++;

	if ((header.free_blocks == 0) && (header.prev)) {
		// slab is empty, and it's not the first;
		header.prev->header.next = nullptr;
		free_memory_to_os(this, sizeof(Slab));
		// The slab committed suicide, don't ever use it again!
	}
}

struct BigSlab {
	u32 magic;
	MemoryBlock<> block;
};

class BigSlabAllocator
{
  public:
	static void* alloc(size_t size)
	{
		BigSlab* mem_block = static_cast<BigSlab*>(request_memory_from_os(size + sizeof(BigSlab)));
		mem_block->block.slab_size = size + sizeof(BigSlab);
		mem_block->block.slab_ptr = mem_block;
		mem_block->magic = MAGIC;

		return mem_block->block.data;
	}

	static void free(BigSlab* slab)
	{
		ASSERT(slab->magic == MAGIC);
		free_memory_to_os(slab, slab->block.slab_size);
	}
};