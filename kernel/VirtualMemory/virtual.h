#include "types.h"

void setup_virtual_memory();
uintptr_t virtual_alloc(uint32_t size, uint32_t flags);
uintptr_t virtual_free(uint32_t size, uint32_t flags);
uintptr_t virtual_map(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);
uintptr_t virtual_unmap(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags);

static uint32_t get_kernel_pages();
