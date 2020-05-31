#include "kernel_map.h"
#include "types.h"

#define MAX_PHYSICAL_4K_PAGES (1024 * 1024)

void initialize_physical_page();
uint32_t alloc_physical_page();
uint32_t alloc_contagious_physical_page();
void free_physical_page(uint32_t page_number);
void set_free_physical_page(uint32_t page_number);
void set_used_physical_page(uint32_t page_number);

extern volatile uint8_t physical_memory_tracer[];