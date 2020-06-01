#include "virtual.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/physical.h"

void setup_virtual_memory()
{

	setup_paging();
	// Reserve Low Pages
	set_used_physical_pages(0, GET_FRAME(KERNEL_PHYSICAL_ADDRESS));
	set_used_physical_pages(GET_FRAME(KERNEL_PHYSICAL_ADDRESS), get_kernel_pages());
	// map kernel pages
	// map_virtual_pages(KERNEL_VIRTUAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, get_kernel_pages());
	// map_virtual_pages(KERNEL_PHYSICAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, get_kernel_pages());
	// map_virtual_page(KERNEL_VIRTUAL_ADDRESS, alloc_physical_page());
}

/*uint32_t get_kernel_pages()
{
    uint32_t kernel_size = (uint32_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
    uint32_t pages = kernel_size / PAGE_4K + ((kernel_size % PAGE_4K == 0) ? 0 : 1);
    return pages;
}*/