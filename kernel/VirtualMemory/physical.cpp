#include "physical.h"

volatile uint8_t physical_memory_tracer[MAX_PHYSICAL_4K_PAGES / 8];

void initialize_physical_mem()
{
	for (size_t i = 0; i < sizeof(physical_memory_tracer) / sizeof(uint8_t); i++) {
		physical_memory_tracer[i] = 0;
	}
}

uint32_t alloc_physical_mem()
{
	for (size_t i = 0; i < sizeof(physical_memory_tracer) / sizeof(uint8_t); i++) {
		if (physical_memory_tracer[i] != 0xFF) {
			uint8_t value = physical_memory_tracer[i];
			for (size_t bit = 0; bit < sizeof(uint8_t); bit++) {
				if (!(value & 1)) {
					uint32_t free_page = bit + i * sizeof(uint8_t);
					set_used_physical_mem(free_page);
					return free_page;
				} else {
					value = value >> 1;
				}
			}
		}
	}
}

uint32_t alloc_contagious_physical_mem()
{
}

uint32_t free_physical_mem(uint32_t page_number, int count)
{
	for (size_t i = 0; i < count; i++) {
		set_free_physical_mem(page_number + i);
	}
}

void set_free_physical_mem(uint32_t page_number)
{
	physical_memory_tracer[page_number / sizeof(uint8_t)] &= ~(1 << (page_number % sizeof(uint8_t)));
}

void set_used_physical_mem(uint32_t page_number)
{
	physical_memory_tracer[page_number / sizeof(uint8_t)] |= 1 << (page_number % sizeof(uint8_t));
}