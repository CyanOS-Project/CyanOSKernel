#include "kernel_map.h"
#include "types.h"

#define MAX_PHYSICAL_4K_PAGES (1024 * 1024)

void initialize_physical_memory();
uint32_t alloc_physical_page();
void free_physical_page(uint32_t page_number);
void set_free_physical_page(uint32_t page_number);
void set_used_physical_page(uint32_t page_number);
void set_used_physical_pages(uint32_t page_number, uint32_t count);

extern volatile uint8_t physical_memory_tracer[];