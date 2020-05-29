#include "boot_paging.h"
#include "kernel_map.h"
#include "paging.h"

__attribute__((section(".boot_pagetable"))) volatile PAGE_DIRECTORY startup_page_direcotry __attribute__((aligned(4)));

void setup_startup_paging()
{
	initialize_page_directory(&startup_page_direcotry);
	// identity mapping
	fill_directory_PSE_entry(&startup_page_direcotry.page_directory_entries[GET_DIRECTORY(KERNEL_PHYSICAL_ADDRESS)],
	                         GET_FRAME(KERNEL_PHYSICAL_ADDRESS), 0, 1);
	// map to higher half
	fill_directory_PSE_entry(&startup_page_direcotry.page_directory_entries[GET_DIRECTORY(KERNEL_VIRTUAL_ADDRESS)],
	                         GET_FRAME(KERNEL_PHYSICAL_ADDRESS), 0, 1);
}