#include "physical.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"

volatile uint8_t PhysicalMemory::physical_memory_tracer[MAX_PHYSICAL_4K_PAGES / 8];
volatile uint32_t PhysicalMemory::physical_memory_size;

void PhysicalMemory::initialize()
{
	for (size_t i = 0; i < sizeof(physical_memory_tracer); i++) {
		physical_memory_tracer[i] = 0;
	}
	physical_memory_size = 0;
}

uint32_t PhysicalMemory::alloc_page()
{
	uint32_t physical_page = find_pages(1);
	set_used_pages(physical_page, 1);
	return physical_page * PAGE_SIZE;
}

uint32_t PhysicalMemory::alloc_contagious_pages(int count)
{
	uint32_t physical_pages = find_pages(count);
	set_used_pages(physical_pages, count);
	return physical_pages * PAGE_SIZE;
}

void PhysicalMemory::free_pages(uint32_t page_number, uint32_t count)
{
	set_free_pages(page_number, count);
}

void PhysicalMemory::set_free_pages(uint32_t page_number, uint32_t count)
{
	uint32_t current_page = page_number;
	for (size_t i = 0; i < count; i++) {
		physical_memory_tracer[current_page / 8] &= ~(1 << (current_page % 8));
		current_page++;
	}
	physical_memory_size -= count * PAGE_SIZE;
}

void PhysicalMemory::set_used_pages(uint32_t page_number, uint32_t count)
{
	uint32_t current_page = page_number;
	for (size_t i = 0; i < count; i++) {
		physical_memory_tracer[current_page / 8] |= 1 << (current_page % 8);
		current_page++;
	}
	physical_memory_size += count * PAGE_SIZE;
}

uint32_t PhysicalMemory::find_pages(uint32_t count)
{
	// TODO: keep a pointer to the last allocated/freed page to improve the performance.
	uint32_t first_free_page = 0;
	uint32_t remaining_pages = count;
	for (size_t i = 0; i < sizeof(physical_memory_tracer); i++) {
		if ((physical_memory_tracer[i] == 0xFF) && (remaining_pages != count)) {
			remaining_pages = count;
		} else {
			for (size_t bit = 0; bit < 8; bit++) {
				if (!CHECK_BIT(physical_memory_tracer[i], bit)) {
					if (remaining_pages == count)
						first_free_page = bit + i * 8;
					if (!(--remaining_pages))
						return first_free_page;
				} else if (remaining_pages != count) {
					remaining_pages = count;
				}
			}
		}
	}
	PANIC("No physical memory available!");
}

uint32_t PhysicalMemory::get_physical_memory_size()
{
	return physical_memory_size;
}
