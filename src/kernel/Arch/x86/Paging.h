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

#define PAGE_FLAGS_PRESENT       Bit(0)
#define PAGE_FLAGS_WRITABLE      Bit(1)
#define PAGE_FLAGS_USER          Bit(2)
#define PAGE_FLAGS_WRITE_THOUGH  Bit(3)
#define PAGE_FLAGS_CACHE_DISABLE Bit(4)
#define PAGE_FLAGS_ACCESSED      Bit(5)
#define PAGE_FLAGS_DIRTY         Bit(6)
#define PAGE_FLAGS_PSE           Bit(7)
#define PAGE_FLAGS_GLOBAL        Bit(8)

// FIXME: use normal bitmask instead of bit fields.
typedef volatile struct PAGE_TABLE_ENTRY_t {
	u32 present : 1;  // Page present in memory
	u32 rw : 1;       // Read-only if clear, readwrite if set
	u32 user : 1;     // Supervisor level only if clear
	u32 pwt : 1;      // Page-level write-through
	u32 pcd : 1;      // Page-level cache disable
	u32 accessed : 1; // Has the page been accessed since last refresh?
	u32 dirty : 1;    // Has the page been written to since last refresh?
	u32 pse : 1;      // 4MB Page
	u32 global : 1;   // page is global
	u32 unused : 3;   // Amalgamation of unused and reserved bits
	u32 frame : 20;   // Frame address (shifted right 12 bits)
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
  public:
	static void setup(u32 num_kernel_pages);
	static void map_pages(u32 virtual_address, u32 physical_address, u32 pages, u32 flags);
	static void map_boot_pages(u32 virtual_address, u32 physical_address, u32 pages);
	static void unmap_pages(u32 virtual_address, u32 pages);
	static bool check_page_present(u32 virtual_address);
	static bool check_page_table_exists(u32 virtual_address);
	static u32 get_physical_page(u32 virtual_address);
	static void load_page_directory(u32 page_direcotry);
	static void map_kernel_pd_entries(u32 pd);
	static void map_page_table(u32 virtual_address, u32 pt);
	static void map_page(u32 virtual_address, u32 physical_address, u32 flags);
	static void unmap_page(u32 virtual_address);

  private:
	static void setup_page_tables();
	static void initialize_page_directory(PAGE_DIRECTORY* page_direcotry);
	static void initialize_page_table(PAGE_TABLE* page_direcotry);
	static void fill_directory_entry(PAGE_DIRECTORY_ENTRY* page_direcotry_entry, u16 physical_frame, u32 flags);
	static void fill_directory_PSE_entry(PAGE_DIRECTORY_ENTRY* page_direcotry_entry, u16 physical_frame, u32 flags);
	static void fill_page_table_entry(PAGE_TABLE_ENTRY* page_table_entry, u16 physical_frame, u32 flags);
	static bool check_page_exits_in_table(u32 virtual_address);
	static void invalidate_page(u32 addr);
	static void enable_PSE();
	static void enable_paging();
	static PAGE_DIRECTORY page_direcotry;
	static PAGE_TABLE kernel_page_tables[];
	static PAGE_TABLE boostrap_page_table[];
};
