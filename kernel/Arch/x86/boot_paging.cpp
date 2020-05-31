#include "boot_paging.h"
#include "kernel_map.h"
#include "paging.h"

PAGE_DIRECTORY startup_page_direcotry __attribute__((aligned(PAGE_4K)));

void setup_startup_paging()
{
	initialize_page_directory((PAGE_DIRECTORY*)VIR_TO_PHY((uint32_t)&startup_page_direcotry));
	// Set recursive entry
	fill_directory_entry((PAGE_DIRECTORY_ENTRY*)VIR_TO_PHY((uint32_t)&startup_page_direcotry.entries[RECURSIVE_ENTRY]),
	                     GET_FRAME(VIR_TO_PHY((uint32_t)&startup_page_direcotry)), 0, 1);
	// identity mapping
	fill_directory_PSE_entry((PAGE_DIRECTORY_ENTRY*)VIR_TO_PHY(
	                             (uint32_t)&startup_page_direcotry.entries[GET_PDE_INDEX(KERNEL_PHYSICAL_ADDRESS)]),
	                         GET_PDE_INDEX(KERNEL_PHYSICAL_ADDRESS), 0, 1);
	// map to 0xC0000000
	fill_directory_PSE_entry((PAGE_DIRECTORY_ENTRY*)VIR_TO_PHY(
	                             (uint32_t)&startup_page_direcotry.entries[GET_PDE_INDEX(KERNEL_VIRTUAL_ADDRESS)]),
	                         GET_PDE_INDEX(KERNEL_PHYSICAL_ADDRESS), 0, 1);

	load_page_directory((PAGE_DIRECTORY*)VIR_TO_PHY((uint32_t)&startup_page_direcotry));
	enable_PSE();

	enable_paging();
}