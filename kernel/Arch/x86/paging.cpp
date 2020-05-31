#include "paging.h"

volatile PAGE_DIRECTORY page_direcotry __attribute__((aligned(PAGE_4K)));

void load_pd()
{
	load_page_directory((volatile PAGE_DIRECTORY*)VIR_TO_PHY((uint32_t)&page_direcotry));
}
void setup_paging()
{

	// Set recursive entry
	initialize_page_directory(&page_direcotry);
	fill_directory_entry(&page_direcotry.entries[RECURSIVE_ENTRY], GET_FRAME(VIR_TO_PHY((uint32_t)&page_direcotry)), 0,
	                     1);
	// Setup all kernel page tables
	for (size_t pde = GET_PDE_INDEX(KERNEL_VIRTUAL_ADDRESS);
	     pde < GET_PDE_INDEX(KERNEL_VIRTUAL_ADDRESS) + GET_NUMBER_OF_DIRECTORIES(KERNEL_SPACE_SIZE); pde++) {
		uint32_t page_table_frame = alloc_physical_page();
		fill_directory_entry(&page_direcotry.entries[pde], page_table_frame, 0, 1);
	}
}

void map_virtual_page(uint32_t virtual_address, uint32_t physical_address)
{
	if (virtual_address == 0xC0101000) {
		asm volatile("nop");
	}

	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	fill_page_table_entry(&page_table->entries[GET_PTE_INDEX(virtual_address)], GET_FRAME(physical_address), 0, 1);
}

void map_virtual_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages)
{
	for (size_t i = 0; i < pages; i++) {
		map_virtual_page(virtual_address + PAGE_4K * i, physical_address + PAGE_4K * i);
	}
}

void initialize_page_directory(volatile PAGE_DIRECTORY* page_direcotry)
{
	for (size_t i = 0; i < NUMBER_OF_PAGE_DIRECOTRY_ENTRIES; i++) {
		*((uint32_t*)&page_direcotry->entries[i]) = 0;
	}
}

void fill_directory_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame, bool user,
                          bool writeable)
{
	page_direcotry_entry->unused = 0;
	page_direcotry_entry->global = 0;
	page_direcotry_entry->pwt = 0;
	page_direcotry_entry->pcd = 0;
	page_direcotry_entry->pse = 0;
	page_direcotry_entry->present = 1;
	page_direcotry_entry->rw = writeable;
	page_direcotry_entry->user = user;
	page_direcotry_entry->frame = physical_frame;
}

void fill_directory_PSE_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame, bool user,
                              bool writeable)
{
	page_direcotry_entry->unused = 0;
	page_direcotry_entry->global = 0;
	page_direcotry_entry->pwt = 0;
	page_direcotry_entry->pcd = 0;
	page_direcotry_entry->pse = 1;
	page_direcotry_entry->present = 1;
	page_direcotry_entry->rw = writeable;
	page_direcotry_entry->user = user;
	page_direcotry_entry->frame = physical_frame;
}

void fill_page_table_entry(volatile PAGE_TABLE_ENTRY* page_table_entry, uint16_t physical_frame, bool user,
                           bool writeable)
{
	page_table_entry->unused = 0;
	page_table_entry->global = 0;
	page_table_entry->pwt = 0;
	page_table_entry->pcd = 0;
	page_table_entry->pse = 0;
	page_table_entry->present = 1;
	page_table_entry->rw = writeable;
	page_table_entry->user = user;
	page_table_entry->frame = physical_frame;
}

void load_page_directory(volatile PAGE_DIRECTORY* page_direcotry)
{
	asm("MOV %0,%%CR3" : : "r"(page_direcotry) :);
}

void enable_paging()
{
	asm("MOV %%CR0, %%EAX;"
	    "OR %0,%%EAX;"
	    "MOV %%EAX, %%CR0;"
	    :
	    : "i"(CR0_WP | CR0_PAGING)
	    : "eax");
}

void enable_PSE()
{
	asm("MOV %%CR4, %%EAX;"
	    "OR %0,%%EAX;"
	    "MOV %%EAX, %%CR4;"
	    :
	    : "i"(CR4_PSE)
	    : "eax");
}

void invalidate_pagetable()
{
}