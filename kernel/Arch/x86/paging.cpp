#include "paging.h"

PAGE_DIRECTORY page_direcotry __attribute__((aligned(PAGE_SIZE)));
PAGE_TABLE startup_page_tables[NUMBER_OF_PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

// Initialize page direcotry and page tables.
void setup_paging(uint32_t num_kernel_pages)
{
	PAGE_DIRECTORY* ph_page_direcotry = (PAGE_DIRECTORY*)VIR_TO_PHY((uint32_t)&page_direcotry);
	PAGE_TABLE* ph_page_tables = (PAGE_TABLE*)VIR_TO_PHY((uint32_t)startup_page_tables);
	initialize_page_directory(ph_page_direcotry);
	// Link to page tables
	uint32_t page_flags = PAGE_FLAGS_PRESENT | PAGE_FLAGS_WRITABLE;
	for (size_t i = 0; i < NUMBER_OF_PAGE_DIRECOTRY_ENTRIES; i++) {
		initialize_page_table(&ph_page_tables[i]);
		fill_directory_entry(&ph_page_direcotry->entries[i], GET_FRAME((uint32_t)&ph_page_tables[i]), page_flags);
	}
	// Set recursive entry
	fill_directory_entry(&ph_page_direcotry->entries[RECURSIVE_ENTRY], GET_FRAME((uint32_t)ph_page_direcotry),
	                     page_flags);
	// Map identity and higher half
	map_boot_pages(KERNEL_PHYSICAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, num_kernel_pages);
	map_boot_pages(KERNEL_VIRTUAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, num_kernel_pages);
	// Load page directory and enable paging
	load_page_directory(ph_page_direcotry);
	enable_PSE();
	enable_paging();
}

// Map virtual address range to physical address, can be used only at boot time.
void map_boot_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages)
{
	PAGE_TABLE* ph_page_tables = (PAGE_TABLE*)VIR_TO_PHY((uint32_t)startup_page_tables);
	uint32_t current_v_page = virtual_address;
	uint32_t current_p_page = physical_address;
	uint32_t page_flags = PAGE_FLAGS_PRESENT | PAGE_FLAGS_WRITABLE;
	for (size_t i = 0; i < pages; i++) {
		uint32_t pde = GET_PDE_INDEX(current_v_page);
		uint32_t pte = GET_PTE_INDEX(current_v_page);
		fill_page_table_entry(&ph_page_tables[pde].entries[pte], GET_FRAME(current_p_page), page_flags);
		current_v_page += PAGE_SIZE;
		current_p_page += PAGE_SIZE;
	}
}

// Map a virtual page to a physical page.
void map_virtual_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags)
{

	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	fill_page_table_entry(&page_table->entries[GET_PTE_INDEX(virtual_address)], GET_FRAME(physical_address), flags);
	invalidate_page(virtual_address);
}

// Map contagious virtual pages to contagious physical pages.
void map_virtual_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages, uint32_t flags)
{
	for (size_t i = 0; i < pages; i++) {
		map_virtual_page(virtual_address + PAGE_SIZE * i, physical_address + PAGE_SIZE * i, flags);
	}
}

// Map a virtual page to a physical page.
void unmap_virtual_page(uint32_t virtual_address)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	page_table->entries[GET_PTE_INDEX(virtual_address)].present = 0;
	invalidate_page(virtual_address);
}

// Map contagious virtual pages to contagious physical pages.
void unmap_virtual_pages(uint32_t virtual_address, uint32_t pages)
{
	for (size_t i = 0; i < pages; i++) {
		unmap_virtual_page(virtual_address + PAGE_SIZE * i);
	}
}

bool check_page_present(uint32_t virtual_address)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	return page_table->entries[GET_PTE_INDEX(virtual_address)].present;
}

uint32_t get_physical_page(uint32_t virtual_address)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	return page_table->entries[GET_PTE_INDEX(virtual_address)].frame;
}

// Zeroing page directory entries.
void initialize_page_directory(volatile PAGE_DIRECTORY* page_direcotry)
{
	for (size_t i = 0; i < NUMBER_OF_PAGE_DIRECOTRY_ENTRIES; i++) {
		*((uint32_t*)&page_direcotry->entries[i]) = 0;
	}
}

// Zeroing page table entries.
void initialize_page_table(volatile PAGE_TABLE* page_direcotry)
{
	for (size_t i = 0; i < NUMBER_OF_PAGE_TABLE_ENTRIES; i++) {
		*((uint32_t*)&page_direcotry->entries[i]) = 0;
	}
}

void fill_directory_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame, uint32_t flags)
{
	page_direcotry_entry->unused = 0;
	page_direcotry_entry->global = BOOL(flags & PAGE_FLAGS_GLOBAL);
	page_direcotry_entry->pwt = 0;
	page_direcotry_entry->pcd = 0;
	page_direcotry_entry->pse = 0;
	page_direcotry_entry->present = BOOL(flags & PAGE_FLAGS_PRESENT);
	page_direcotry_entry->rw = BOOL(flags & PAGE_FLAGS_WRITABLE);
	page_direcotry_entry->user = BOOL(flags & PAGE_FLAGS_USER);
	page_direcotry_entry->frame = physical_frame;
}

void fill_directory_PSE_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame,
                              uint32_t flags)
{
	page_direcotry_entry->unused = 0;
	page_direcotry_entry->global = BOOL(flags & PAGE_FLAGS_GLOBAL);
	page_direcotry_entry->pwt = 0;
	page_direcotry_entry->pcd = 0;
	page_direcotry_entry->pse = 1;
	page_direcotry_entry->present = BOOL(flags & PAGE_FLAGS_PRESENT);
	page_direcotry_entry->rw = BOOL(flags & PAGE_FLAGS_WRITABLE);
	page_direcotry_entry->user = BOOL(flags & PAGE_FLAGS_USER);
	page_direcotry_entry->frame = physical_frame;
}

void fill_page_table_entry(volatile PAGE_TABLE_ENTRY* page_table_entry, uint16_t physical_frame, uint32_t flags)
{
	page_table_entry->unused = 0;
	page_table_entry->global = BOOL(flags & PAGE_FLAGS_GLOBAL);
	page_table_entry->pwt = 0;
	page_table_entry->pcd = 0;
	page_table_entry->pse = 0;
	page_table_entry->present = BOOL(flags & PAGE_FLAGS_PRESENT);
	page_table_entry->rw = BOOL(flags & PAGE_FLAGS_WRITABLE);
	page_table_entry->user = BOOL(flags & PAGE_FLAGS_USER);
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

void invalidate_page(uint32_t addr)
{
	asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}