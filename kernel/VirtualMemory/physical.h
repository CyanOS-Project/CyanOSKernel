#include "types.h"

#define MAX_PHYSICAL_4K_PAGES (1024 * 1024)

void initialize_physical_mem();
uint32_t alloc_physical_mem();
uint32_t alloc_contagious_physical_mem();
uint32_t free_physical_mem(uint32_t page_number, int count);
void set_free_physical_mem(uint32_t page_number);
void set_used_physical_mem(uint32_t page_number);

extern volatile uint8_t physical_memory_tracer[];