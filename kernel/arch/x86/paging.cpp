
#include "paging.h"

volatile PAGE_DIRECTORY page_direcotry __attribute__((aligned(4)));
volatile PAGE_TABLE page_tables[GET_NUMBER_OF_DIRECTORIES(MAX_KERNEL_SIZE)];

void setup_paging()
{
	map_identity(&page_direcotry, page_tables);
	load_page_directory(&page_direcotry);
	// enable_paging();
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
	    : "i"(0x80010000));
}
static void map_identity(volatile PAGE_DIRECTORY* page_direcotry, volatile PAGE_TABLE* page_tables)
{
	for (size_t dir = 0; dir < get_kernel_directories(); dir++) {
		fill_directory_entry(&page_direcotry->page_directory_entries[dir], GET_FRAME((uint32_t)&page_tables[dir]), 0,
		                     1);
		for (size_t page = 0; page < NUMBER_OF_PAGE_TABLE_ENTRIES; page++) {
			fill_page_table_entry(&page_tables[dir].page_table_entries[dir], GET_PAGE_VIRTUAL_ADDRESS(dir, page), 0, 1);
		}
	}
}
uint32_t get_kernel_directories()
{
	volatile uint32_t _KERNEL_END = (uint32_t)&KERNEL_END;
	return GET_NUMBER_OF_DIRECTORIES(_KERNEL_END) + (GET_PAGE(_KERNEL_END) ? 1 : 0);
}

void fill_directory_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame, bool user,
                          bool writeable)
{
	page_direcotry_entry->present = 1;
	page_direcotry_entry->rw = writeable;
	page_direcotry_entry->user = user;
	page_direcotry_entry->frame = physical_frame;
}
void fill_page_table_entry(volatile PAGE_TABLE_ENTRY* page_table_entry, uint16_t physical_frame, bool user,
                           bool writeable)
{
	page_table_entry->present = 1;
	page_table_entry->rw = writeable;
	page_table_entry->user = user;
	page_table_entry->frame = physical_frame;
}

void invalidate_pagetable()
{
}