#pragma once

#include "Kernel_map.h"
#include <Clib.h>
#include <Types.h>

#define NUMBER_OF_PAGE_DIRECOTRY_ENTRIES 1024
#define NUMBER_OF_PAGE_TABLE_ENTRIES     1024
#define FULL_KERNEL_PAGES                (NUMBER_OF_PAGE_DIRECOTRY_ENTRIES - GET_NUMBER_OF_DIRECTORIES(KERNEL_BASE))
#define RECURSIVE_ENTRY                  (NUMBER_OF_PAGE_DIRECOTRY_ENTRIES - 1)

#define CR4_PSE    (1 << 4)
#define CR0_PAGING (1 << 31)
#define CR0_WP     (1 << 16)

#define GET_FRAME(x)                                    (x >> 12)
#define GET_PTE_INDEX(x)                                ((x >> 12) & 0x3FF)
#define GET_PDE_INDEX(x)                                (x >> 22)
#define GET_NUMBER_OF_DIRECTORIES(x)                    (x >> 22)
#define GET_OFFSET(x)                                   (x & 0xFFF)
#define GET_PAGE_VIRTUAL_ADDRESS(dir_index, page_index) (((dir_index) << 22) | ((page_index) << 12))
#define BOOL(x)                                         ((bool)(x))

#define GET_PAGES(x)  (x / PAGE_SIZE + ((x % PAGE_SIZE) == 0 ? 0 : 1))
#define PAGE_ALIGN(x) (GET_PAGES(x) * PAGE_SIZE)

#define PAGE_SIZE          0x1000
#define FIRST_PAGE_ADDRESS 0x1000
#define LAST_PAGE_ADDRESS  GET_PAGE_VIRTUAL_ADDRESS(RECURSIVE_ENTRY - 1, NUMBER_OF_PAGE_TABLE_ENTRIES - 1)

#define PAGE_FLAGS_PRESENT  1
#define PAGE_FLAGS_USER     2
#define PAGE_FLAGS_WRITABLE 4
#define PAGE_FLAGS_GLOBAL   8

typedef volatile struct PAGE_TABLE_ENTRY_t {
	uint32_t present : 1;  // Page present in memory
	uint32_t rw : 1;       // Read-only if clear, readwrite if set
	uint32_t user : 1;     // Supervisor level only if clear
	uint32_t pwt : 1;      // Page-level write-through
	uint32_t pcd : 1;      // Page-level cache disable
	uint32_t accessed : 1; // Has the page been accessed since last refresh?
	uint32_t dirty : 1;    // Has the page been written to since last refresh?
	uint32_t pse : 1;      // 4MB Page
	uint32_t global : 1;   // page is global
	uint32_t unused : 3;   // Amalgamation of unused and reserved bits
	uint32_t frame : 20;   // Frame address (shifted right 12 bits)
} PAGE_TABLE_ENTRY;

typedef PAGE_TABLE_ENTRY PAGE_DIRECTORY_ENTRY;

typedef volatile struct PAGE_DIRECTORY_t {
	PAGE_DIRECTORY_ENTRY entries[NUMBER_OF_PAGE_DIRECOTRY_ENTRIES];
} PAGE_DIRECTORY;

typedef volatile struct PAGE_TABLE_t {
	PAGE_TABLE_ENTRY entries[NUMBER_OF_PAGE_TABLE_ENTRIES];
} PAGE_TABLE;

class Paging
{
  private:
	static void setup_page_tables();
	static void initialize_page_directory(PAGE_DIRECTORY* page_direcotry);
	static void initialize_page_table(PAGE_TABLE* page_direcotry);
	static void fill_directory_entry(PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame,
	                                 uint32_t flags);
	static void fill_directory_PSE_entry(PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame,
	                                     uint32_t flags);
	static void fill_page_table_entry(PAGE_TABLE_ENTRY* page_table_entry, uint16_t physical_frame, uint32_t flags);
	static bool check_page_exits_in_table(uint32_t virtual_address);
	static void invalidate_page(uint32_t addr);
	static void enable_PSE();
	static void enable_paging();
	static PAGE_DIRECTORY page_direcotry;
	static PAGE_TABLE kernel_page_tables[];
	static PAGE_TABLE boostrap_page_table[];

  public:
	static void setup(uint32_t num_kernel_pages);
	static void map_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages, uint32_t flags);
	static void map_boot_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages);
	static void unmap_pages(uint32_t virtual_address, uint32_t pages);
	static bool check_page_present(uint32_t virtual_address);
	static bool check_page_table_exists(uint32_t virtual_address);
	static uint32_t get_physical_page(uint32_t virtual_address);
	static void load_page_directory(uint32_t page_direcotry);
	static void map_kernel_pd_entries(uint32_t pd);
	static void map_page_table(uint32_t virtual_address, uint32_t pt);
	static void map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags);
	static void unmap_page(uint32_t virtual_address);
};
