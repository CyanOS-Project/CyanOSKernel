#include "virtual.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/physical.h"

void setup_virtual_memory()
{
	/*// Reserve Low Pages
	for (size_t i = 0; i < GET_FRAME(KERNEL_PHYSICAL_ADDRESS); i++) {
	    set_used_physical_page(i);
	}
	// Reserve Kernel pages
	for (size_t i = 0; i < get_kernel_pages(); i++) {
	    set_used_physical_page(GET_FRAME(KERNEL_PHYSICAL_ADDRESS) + i);
	}

	setup_paging();
	// map kernel pages
	map_virtual_pages(KERNEL_VIRTUAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, get_kernel_pages());
	// map_virtual_page(KERNEL_VIRTUAL_ADDRESS, alloc_physical_page());
	load_pd();*/
}

/*uint32_t get_kernel_pages()
{
    uint32_t kernel_size = (uint32_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
    uint32_t pages = kernel_size / PAGE_4K + ((kernel_size % PAGE_4K == 0) ? 0 : 1);
    return pages;
}*/