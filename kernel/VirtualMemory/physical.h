#include "kernel_map.h"
#include "types.h"

#define MAX_PHYSICAL_4K_PAGES (1024 * 1024)
#define CHECK_BIT(value, bit) ((value >> bit) & 1)

void initialize_physical_memory();
uint32_t alloc_physical_page();
uint32_t alloc_contagious_physical_pages(int count);
void free_physical_pages(uint32_t page_number, uint32_t count);
void set_free_physical_pages(uint32_t page_number, uint32_t count);
void set_used_physical_pages(uint32_t page_number, uint32_t count);
uint32_t find_physical_pages(uint32_t count);
