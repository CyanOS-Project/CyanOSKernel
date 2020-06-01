#pragma once

#include "kernel_map.h"
#include "physical.h"
#include "types.h"

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

void setup_paging();
void initialize_page_directory(volatile PAGE_DIRECTORY* page_direcotry);
void map_virtual_page(uint32_t virtual_address, uint32_t physical_address);
void map_virtual_pages(uint32_t virtual_address, uint32_t physical_address, uint32_t pages);
void map_boot_pages(uint32_t virtual_address, uint32_t physical_address, int pages);
void enable_PSE();    // TODO: make it inline
void enable_paging(); // TODO: make it inline
void load_page_directory(volatile PAGE_DIRECTORY* page_direcotry);
void invalidate_pagetable();
uint32_t get_kernel_pages();

uint32_t get_kernel_directories();
void fill_directory_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame, bool user,
                          bool writeable);
void fill_page_table_entry(volatile PAGE_TABLE_ENTRY* page_table_entry, uint16_t physical_frame, bool user,
                           bool writeable);
void fill_directory_PSE_entry(volatile PAGE_DIRECTORY_ENTRY* page_direcotry_entry, uint16_t physical_frame, bool user,
                              bool writeable);
extern unsigned KERNEL_END;
