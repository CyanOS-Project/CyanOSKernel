#include "virtual.h"
#include "Arch/x86/paging.h"
#include "physical.h"

void setup_virtual_memory()
{
	setup_paging(get_kernel_pages());
	initialize_physical_memory();
	// Reserve Low 1MB Pages.
	set_used_physical_pages(0, GET_FRAME(KERNEL_PHYSICAL_ADDRESS));
	// Set kernel memory as used.
	set_used_physical_pages(GET_FRAME(KERNEL_PHYSICAL_ADDRESS), get_kernel_pages());
}

uintptr_t virtual_alloc(uint32_t size, uint32_t flags)
{
}

uintptr_t virtual_free(uint32_t size, uint32_t flags)
{
}

uintptr_t virtual_map(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags)
{
	uint32_t pages_num = size / PAGE_4K + ((size % PAGE_4K == 0) ? 0 : 1);
	map_virtual_pages(virtual_address, physical_address, pages_num);
}

uintptr_t virtual_unmap(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags)
{
}

static uint32_t get_kernel_pages()
{
	uint32_t kernel_size = (uint32_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
	uint32_t pages = kernel_size / PAGE_4K + ((kernel_size % PAGE_4K == 0) ? 0 : 1);
	return pages;
}
