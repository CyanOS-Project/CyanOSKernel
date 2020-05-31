#include "physical.h"

volatile uint8_t physical_memory_tracer[MAX_PHYSICAL_4K_PAGES / 8];

void initialize_physical_page()
{
	for (size_t i = 0; i < sizeof(physical_memory_tracer) / sizeof(uint8_t); i++) {
		physical_memory_tracer[i] = 0;
	}
}

uint32_t alloc_physical_page()
{
	for (size_t i = 0; i < sizeof(physical_memory_tracer) / sizeof(uint8_t); i++) {
		if (physical_memory_tracer[i] != 0xFF) {
			uint8_t value = physical_memory_tracer[i];
			for (size_t bit = 0; bit < sizeof(uint8_t); bit++) {
				if (!(value & 1)) {
					uint32_t free_page = bit + i * sizeof(uint8_t);
					set_used_physical_page(free_page);
					return free_page;
				} else {
					value = value >> 1;
				}
			}
		}
	}
}

uint32_t alloc_contagious_physical_page(int count)
{
	/*int remaining_pages = count;
	int start_page = 0;
	for (size_t i = 0; i < sizeof(physical_memory_tracer) / sizeof(uint8_t); i++) {
	    if (physical_memory_tracer[i] != 0xFF) {
	        uint8_t value = physical_memory_tracer[i];
	        for (size_t bit = 0; bit < sizeof(uint8_t); bit++) {
	            if (!(value & 1)) {
	                remaining_pages--;
	                if (!remaining_pages) {
	                    uint32_t free_page = bit + i * sizeof(uint8_t);
	                    set_used_physical_page(free_page);
	                    return free_page;
	                }
	            } else {
	                remaining_pages = count;
	                value = value >> 1;
	            }
	        }
	    }
	}*/
}

void free_physical_page(uint32_t page_number)
{
	set_free_physical_page(page_number);
}

void set_free_physical_page(uint32_t page_number)
{
	physical_memory_tracer[page_number / sizeof(uint8_t)] &= ~(1 << (page_number % sizeof(uint8_t)));
}

void set_used_physical_page(uint32_t page_number)
{
	physical_memory_tracer[page_number / sizeof(uint8_t)] |= 1 << (page_number % sizeof(uint8_t));
}