#include "memory.h"
#include "Arch/x86/paging.h"
#include "physical.h"
#include "virtual.h"

void setup_virtual_memory()
{
	setup_paging(get_kernel_pages());
	initialize_physical_memory();
	// Reserve Low 1MB Pages.
	set_used_physical_pages(0, GET_FRAME(KERNEL_PHYSICAL_ADDRESS));
	// Set kernel memory as used.
	set_used_physical_pages(GET_FRAME(KERNEL_PHYSICAL_ADDRESS), get_kernel_pages());
}

uintptr_t memory_alloc(uint32_t size, uint32_t flags)
{
	uint32_t vAdd;
	if (flags & MEMORY::KERNEL) {
		vAdd = virtual_find_pages(GET_FRAME(KERNEL_VIRTUAL_ADDRESS), 1024 * 1024, 1);
	} else {
		vAdd = virtual_find_pages(1, GET_FRAME(KERNEL_VIRTUAL_ADDRESS), 1); // skip first page to detect null pointer
	}
	uint32_t pAdd = alloc_physical_page();
	map_virtual_pages(vAdd, pAdd, 1);
}

uintptr_t memory_free(uintptr_t address, uint32_t size, uint32_t flags)
{
	free_physical_pages(get_physical_page(address), size);
	unmap_virtual_pages(address, size);
}

uintptr_t memory_map(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags)
{
	uint32_t pages_num = size / PAGE_4K + ((size % PAGE_4K == 0) ? 0 : 1);
	map_virtual_pages(virtual_address, physical_address, pages_num);
}

uintptr_t memory_unmap(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags)
{
}

uint32_t virtual_memory_size()
{
}

uint32_t physical_memory_size()
{
}

static uint32_t get_kernel_pages()
{
	uint32_t kernel_size = (uint32_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
	uint32_t pages = kernel_size / PAGE_4K + ((kernel_size % PAGE_4K == 0) ? 0 : 1);
	return pages;
}
