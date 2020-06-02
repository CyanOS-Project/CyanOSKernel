#include "physical.h"
#include "Arch/x86/panic.h"

volatile uint8_t physical_memory_tracer[MAX_PHYSICAL_4K_PAGES / 8];

void initialize_physical_memory()
{
	for (size_t i = 0; i < sizeof(physical_memory_tracer); i++) {
		physical_memory_tracer[i] = 0;
	}
}

uint32_t alloc_physical_page()
{
	// TODO: do function to find page and refactor it
	for (size_t i = 0; i < sizeof(physical_memory_tracer); i++) {
		if (physical_memory_tracer[i] == 0xFF)
			continue;
		uint8_t value = physical_memory_tracer[i];
		for (size_t bit = 0; bit < 8; bit++) {
			if (value & 1) {
				value = value >> 1;
				continue;
			}
			uint32_t free_page = bit + i * 8;
			set_used_physical_page(free_page);
			return free_page;
		}
	}
	PANIC("No physical memory available!");
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

void free_physical_pages(uint32_t page_number, uint32_t count)
{
	for (size_t i = 0; i < count; i++) {
		free_physical_page(page_number + i);
	}
}

void set_free_physical_page(uint32_t page_number)
{
	physical_memory_tracer[page_number / 8] &= ~(1 << (page_number % 8));
}

void set_used_physical_page(uint32_t page_number)
{
	physical_memory_tracer[page_number / 8] |= 1 << (page_number % 8);
}

void set_used_physical_pages(uint32_t page_number, uint32_t count)
{
	uint32_t current_page = page_number;
	for (size_t i = 0; i < count; i++) {
		physical_memory_tracer[current_page / 8] |= 1 << (current_page % 8);
		current_page++;
	}
}