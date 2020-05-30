#include "boot_paging.h"
#include "kernel_map.h"
#include "paging.h"

volatile PAGE_DIRECTORY startup_page_direcotry __attribute__((aligned(PAGE_4K)));

void setup_startup_paging()
{
	initialize_page_directory((volatile PAGE_DIRECTORY*)VIR_TO_PHY((uint32_t)&startup_page_direcotry));
	// identity mapping
	fill_directory_PSE_entry(
	    (volatile PAGE_DIRECTORY_ENTRY*)VIR_TO_PHY(
	        (uint32_t)&startup_page_direcotry.page_directory_entries[GET_DIRECTORY(KERNEL_PHYSICAL_ADDRESS)]),
	    GET_DIRECTORY(KERNEL_PHYSICAL_ADDRESS), 0, 1);
	// map to 0xC0000000
	fill_directory_PSE_entry(
	    (volatile PAGE_DIRECTORY_ENTRY*)VIR_TO_PHY(
	        (uint32_t)&startup_page_direcotry.page_directory_entries[GET_DIRECTORY(KERNEL_VIRTUAL_ADDRESS)]),
	    GET_DIRECTORY(KERNEL_PHYSICAL_ADDRESS), 0, 1);

	load_page_directory((volatile PAGE_DIRECTORY*)VIR_TO_PHY((uint32_t)&startup_page_direcotry));
	enable_PSE();

	enable_paging();
}