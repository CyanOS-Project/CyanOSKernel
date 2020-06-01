#include "paging.h"

PAGE_DIRECTORY page_direcotry __attribute__((aligned(PAGE_4K)));
PAGE_TABLE startup_page_tables[NUMBER_OF_PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_4K)));

void setup_paging()
{
	PAGE_DIRECTORY* ph_page_direcotry = (PAGE_DIRECTORY*)VIR_TO_PHY((uint32_t)&page_direcotry);
	PAGE_TABLE* ph_page_tables = (PAGE_TABLE*)VIR_TO_PHY((uint32_t)startup_page_tables);
	initialize_physical_page();
	initialize_page_directory(ph_page_direcotry);
	// Link to page tables
	for (size_t i = 0; i < NUMBER_OF_PAGE_DIRECOTRY_ENTRIES; i++) {
		fill_directory_entry(&ph_page_direcotry->entries[i], GET_FRAME((uint32_t)&ph_page_tables[i]), 0, 1);
	}
	// Set recursive entry
	fill_directory_entry(&ph_page_direcotry->entries[RECURSIVE_ENTRY], GET_FRAME((uint32_t)ph_page_direcotry), 0, 1);
	// Map identity and higher half
	map_boot_pages(KERNEL_PHYSICAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, get_kernel_pages());
	map_boot_pages(KERNEL_VIRTUAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, get_kernel_pages());
	// Load page directory and enable paging
	load_page_directory(ph_page_direcotry);
	enable_PSE();
	enable_paging();
}

void map_boot_pages(uint32_t virtual_address, uint32_t physical_address, int pages)
{
	PAGE_TABLE* ph_page_tables = (PAGE_TABLE*)VIR_TO_PHY((uint32_t)startup_page_tables);
	uint32_t current_v_page = virtual_address;
	uint32_t current_p_page = physical_address;
	for (size_t i = 0; i < pages; i++) {
		uint32_t pde = GET_PDE_INDEX(current_v_page);
		uint32_t pte = GET_PTE_INDEX(current_v_page);
		fill_page_table_entry(&ph_page_tables[pde].entries[pte], GET_FRAME(current_p_page), 0, 1);
		set_used_physical_page(GET_FRAME(current_p_page));
		current_v_page += PAGE_4K;
		current_p_page += PAGE_4K;
	}
}

void map_virtual_page(uint32_t virtual_address, uint32_t physical_address)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	fill_page_table_entry(&page_table->entries[GET_PTE_INDEX(virtual_address)], GET_FRAME(physical_address), 0, 1);
	invalidate_page(virtual_address);
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

uint32_t get_kernel_pages()
{

	uint32_t kernel_size = (uint32_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
	uint32_t pages = kernel_size / PAGE_4K + ((kernel_size % PAGE_4K == 0) ? 0 : 1);
	return pages;
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

void invalidate_page(uint32_t addr)
{
	asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}