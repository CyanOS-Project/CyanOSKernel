#include "virtual.h"
#include "Arch/x86/paging.h"
#include "physical.h"

void setup_virtual_memory()
{
	// setup_paging();
	initialize_physical_mem();
	volatile int mem_a = alloc_physical_mem();
	volatile int mem_b = alloc_physical_mem();
	volatile int mem_c = alloc_physical_mem();
	free_physical_mem(mem_b, 1);
	volatile int mem_d = alloc_physical_mem();
	volatile int mem_e = alloc_physical_mem();
}