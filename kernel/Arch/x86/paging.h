#pragma once

#include "kernel_map.h"
#include "utils/types.h"

#define NUMBER_OF_PAGE_DIRECOTRY_ENTRIES 1024
#define NUMBER_OF_PAGE_TABLE_ENTRIES     1024
#define RECURSIVE_ENTRY                  (NUMBER_OF_PAGE_DIRECOTRY_ENTRIES - 1)

#define CR4_PSE    (1 << 4)
#define CR0_PAGING (1 << 31)
#define CR0_WP     (1 << 16)

#define GET_FRAME(x)                                    (x >> 12)
#define GET_PTE_INDEX(x)                                ((x >> 12) & 0x3FF)
#define GET_PDE_INDEX(x)                                (x >> 22)
#define GET_NUMBER_OF_DIRECTORIES(x)                    (x >> 22)
#define GET_OFFSET(x)                                   (x & 0xFFF)
#define GET_PAGE_VIRTUAL_ADDRESS(dir_index, page_index) ((dir_index << 22) | (page_index << 12))
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

struct PAGE_TABLE_ENTRY {
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
};
typedef PAGE_TABLE_ENTRY PAGE_DIRECTORY_ENTRY;

struct PAGE_DIRECTORY {
	PAGE_DIRECTORY_ENTRY entries[NUMBER_OF_PAGE_DIRECOTRY_ENTRIES];
};

struct PAGE_TABLE {
	PAGE_TABLE_ENTRY entries[NUMBER_OF_PAGE_TABLE_ENTRIES];
};

uint32_t get_physical_page(uint32_t virtual_address);

class Paging
{
  private:
	static void initialize_page_directory(volatile PAGE_DIRECTORY* page_direcotry);
	static void initialize_page_table(volatile PAGE_TABLE* page_direcotry);
	static void map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags);
	static void unmap_page(uint32_t virtual_address);
	static void fill_directory_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame,
	                                 uint32_t flags);
	static void fill_directory_PSE_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame,
	                                     uint32_t flags);
	static void fill_page_table_entry(volatile PAGE_TABLE_ENTRY* page_table_entry, uint16_t physical_frame,
	                                  uint32_t flags);
	static void invalidate_page(uint32_t addr);
	static void enable_PSE();
	static void enable_paging();
	static PAGE_DIRECTORY page_direcotry;
	static PAGE_TABLE startup_page_tables[];

  public:
	static void setup(uint32_t num_kernel_pages);
	static void map_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages, uint32_t flags);
	static void map_boot_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages);
	static void unmap_pages(uint32_t virtual_address, uint32_t pages);
	static bool check_page_present(uint32_t virtual_address);
	static uint32_t get_physical_page(uint32_t virtual_address);
	static void load_page_directory(uint32_t page_direcotry);
};
