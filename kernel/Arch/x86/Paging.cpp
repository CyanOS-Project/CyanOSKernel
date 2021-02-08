#include "Paging.h"

PAGE_DIRECTORY Paging::page_direcotry __attribute__((aligned(PAGE_SIZE)));
PAGE_TABLE Paging::kernel_page_tables[FULL_KERNEL_PAGES] __attribute__((aligned(PAGE_SIZE)));
PAGE_TABLE Paging::boostrap_page_table[2] __attribute__((aligned(PAGE_SIZE)));

// Initialize page direcotry and page tables.
void Paging::setup(u32 num_kernel_pages)
{
	PAGE_DIRECTORY* ph_page_direcotry = (PAGE_DIRECTORY*)VIR_TO_PHY((u32)&page_direcotry);
	initialize_page_directory(ph_page_direcotry);
	// Link to page tables
	setup_page_tables();
	// Map identity and higher half
	map_boot_pages(KERNEL_PHYSICAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, num_kernel_pages);
	map_boot_pages(KERNEL_VIRTUAL_ADDRESS, KERNEL_PHYSICAL_ADDRESS, num_kernel_pages);
	// Load page directory and enable paging
	load_page_directory((u32)ph_page_direcotry);
	enable_PSE();
	enable_paging();
}

void Paging::setup_page_tables()
{
	PAGE_DIRECTORY* ph_page_direcotry = (PAGE_DIRECTORY*)VIR_TO_PHY((u32)&page_direcotry);
	PAGE_TABLE* ph_kpage_tables = (PAGE_TABLE*)VIR_TO_PHY((u32)kernel_page_tables);
	PAGE_TABLE* ph_bootpage_tables = (PAGE_TABLE*)VIR_TO_PHY((u32)boostrap_page_table);
	u32 page_flags = PAGE_FLAGS_PRESENT | PAGE_FLAGS_WRITABLE;
	initialize_page_directory(ph_page_direcotry);
	// Kernel pages tables
	for (size_t i = 0; i < FULL_KERNEL_PAGES; i++) {
		initialize_page_table(&ph_kpage_tables[i]);
		fill_directory_entry(&ph_page_direcotry->entries[i + GET_PDE_INDEX(KERNEL_BASE)],
		                     GET_FRAME((u32)&ph_kpage_tables[i]), page_flags);
	}
	// Identity page tables for 8mb of the kernel bootstrap
	for (size_t i = 0; i < 2; i++) {
		initialize_page_table(&ph_bootpage_tables[i]);
		fill_directory_entry(&ph_page_direcotry->entries[i], GET_FRAME((u32)&ph_bootpage_tables[i]), page_flags);
	}
	// Set recursive entry
	fill_directory_entry(&ph_page_direcotry->entries[RECURSIVE_ENTRY], GET_FRAME((u32)ph_page_direcotry), page_flags);
}
// Map virtual address range to physical address, can be used only at boot time.
void Paging::map_boot_pages(u32 virtual_address, u32 physical_address, u32 pages)
{
	PAGE_DIRECTORY* ph_page_directory = (PAGE_DIRECTORY*)VIR_TO_PHY((u32)&page_direcotry);
	u32 current_v_page = virtual_address;
	u32 current_p_page = physical_address;
	u32 page_flags = PAGE_FLAGS_PRESENT | PAGE_FLAGS_WRITABLE | PAGE_FLAGS_GLOBAL;
	for (size_t i = 0; i < pages; i++) {
		u32 pde = GET_PDE_INDEX(current_v_page);
		u32 pte = GET_PTE_INDEX(current_v_page);
		PAGE_TABLE* ph_page_table = (PAGE_TABLE*)(ph_page_directory->entries[pde].frame * PAGE_SIZE);
		fill_page_table_entry(&ph_page_table->entries[pte], GET_FRAME(current_p_page), page_flags);
		current_v_page += PAGE_SIZE;
		current_p_page += PAGE_SIZE;
	}
}

// Map a virtual page to a physical page.
void Paging::map_page(u32 virtual_address, u32 physical_address, u32 flags)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	fill_page_table_entry(&page_table->entries[GET_PTE_INDEX(virtual_address)], GET_FRAME(physical_address), flags);
	invalidate_page(virtual_address);
}

// Map contagious virtual pages to contagious physical pages.
void Paging::map_pages(u32 virtual_address, u32 physical_address, u32 pages, u32 flags)
{
	for (size_t i = 0; i < pages; i++) {
		map_page(virtual_address + PAGE_SIZE * i, physical_address + PAGE_SIZE * i, flags);
	}
}

// Map a virtual page to a physical page.
void Paging::unmap_page(u32 virtual_address)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	page_table->entries[GET_PTE_INDEX(virtual_address)].present = 0;
	invalidate_page(virtual_address);
}

// Map contagious virtual pages to contagious physical pages.
void Paging::unmap_pages(u32 virtual_address, u32 pages)
{
	for (size_t i = 0; i < pages; i++) {
		unmap_page(virtual_address + PAGE_SIZE * i);
	}
}

void Paging::map_page_table(u32 virtual_address, u32 pt_virtual_address)
{
	u32 page_flags = PAGE_FLAGS_PRESENT | PAGE_FLAGS_WRITABLE | PAGE_FLAGS_USER;
	PAGE_DIRECTORY* page_dir = (PAGE_DIRECTORY*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, RECURSIVE_ENTRY);
	u32 pt_frame = get_physical_page(pt_virtual_address);
	fill_directory_entry(&page_dir->entries[GET_PDE_INDEX(virtual_address)], pt_frame, page_flags);
}

bool Paging::check_page_table_exists(u32 virtual_address)
{
	PAGE_DIRECTORY* page_dir = (PAGE_DIRECTORY*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, RECURSIVE_ENTRY);
	return page_dir->entries[GET_PDE_INDEX(virtual_address)].present;
}

bool Paging::check_page_exits_in_table(u32 virtual_address)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	return page_table->entries[GET_PTE_INDEX(virtual_address)].present;
}

bool Paging::check_page_present(u32 virtual_address)
{
	if (check_page_table_exists(virtual_address) == false)
		return false;
	return check_page_exits_in_table(virtual_address);
}

u32 Paging::get_physical_page(u32 virtual_address)
{
	PAGE_TABLE* page_table = (PAGE_TABLE*)GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY, GET_PDE_INDEX(virtual_address));
	return page_table->entries[GET_PTE_INDEX(virtual_address)].frame;
}

// Zeroing page directory entries.
void Paging::initialize_page_directory(PAGE_DIRECTORY* page_direcotry)
{
	for (size_t i = 0; i < NUMBER_OF_PAGE_DIRECOTRY_ENTRIES; i++) {
		*((u32*)&page_direcotry->entries[i]) = 0;
	}
}

// Zeroing page table entries.
void Paging::initialize_page_table(PAGE_TABLE* page_direcotry)
{
	for (size_t i = 0; i < NUMBER_OF_PAGE_TABLE_ENTRIES; i++) {
		*((u32*)&page_direcotry->entries[i]) = 0;
	}
}

void Paging::map_kernel_pd_entries(u32 pd)
{
	u32 page_flags = PAGE_FLAGS_PRESENT | PAGE_FLAGS_WRITABLE;
	PAGE_DIRECTORY* new_page_dir = (PAGE_DIRECTORY*)pd;
	initialize_page_directory(new_page_dir);
	const size_t start = GET_PDE_INDEX(KERNEL_VIRTUAL_ADDRESS);
	for (size_t i = start; i < RECURSIVE_ENTRY; i++) {
		memcpy((void*)&new_page_dir->entries[i], (void*)&page_direcotry.entries[i], sizeof(PAGE_DIRECTORY_ENTRY));
	}
	fill_directory_entry(&new_page_dir->entries[RECURSIVE_ENTRY], get_physical_page(pd), page_flags);
}

void Paging::fill_directory_entry(PAGE_DIRECTORY_ENTRY* page_direcotry_entry, u16 physical_frame, u32 flags)
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

void Paging::fill_directory_PSE_entry(PAGE_DIRECTORY_ENTRY* page_direcotry_entry, u16 physical_frame, u32 flags)
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

void Paging::fill_page_table_entry(PAGE_TABLE_ENTRY* page_table_entry, u16 physical_frame, u32 flags)
{
	// FIXME: accessing bits directly is so slow.
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

void Paging::load_page_directory(u32 page_direcotry)
{
	asm volatile("movl %0, %%cr3" : : "r"(page_direcotry) :);
}

void Paging::enable_paging()
{
	asm volatile("movl %%cr0, %%eax;"
	             "or  %0, %%eax;"
	             "movl %%eax, %%cr0 ;"
	             :
	             : "i"(CR0_WP | CR0_PAGING)
	             : "eax");
}

void Paging::enable_PSE()
{
	asm volatile("movl %%cr4, %%eax;"
	             "or %0, %%eax;"
	             "movl %%eax, %%cr4;"
	             :
	             : "i"(CR4_PSE)
	             : "eax");
}

void Paging::invalidate_page(u32 addr)
{
	asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}