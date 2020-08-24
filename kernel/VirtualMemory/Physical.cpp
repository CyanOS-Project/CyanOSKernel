#include "Physical.h"

volatile uint8_t PhysicalMemory::physical_memory_tracer[MAX_PHYSICAL_4K_PAGES / 8];
volatile unsigned PhysicalMemory::physical_memory_size;

void PhysicalMemory::initialize()
{
	for (size_t i = 0; i < sizeof(physical_memory_tracer); i++) {
		physical_memory_tracer[i] = 0;
	}
	physical_memory_size = 0;
}

bool PhysicalMemory::check_free_pages(uintptr_t page_number, unsigned count)
{
	size_t index = page_number;
	while (index < page_number + count) {
		if (TEST_BIT(physical_memory_tracer[index / 8], (index % 8))) {
			return false;
		}
		index++;
	}
	return true;
}

uintptr_t PhysicalMemory::alloc_page(uintptr_t start)
{
	uintptr_t physical_page = find_pages(start, 1);
	set_used_pages(physical_page, 1);
	return physical_page * PAGE_SIZE;
}

uintptr_t PhysicalMemory::alloc_contagious_pages(uintptr_t start, size_t count)
{
	uintptr_t physical_pages = find_pages(start, count);
	set_used_pages(physical_pages, count);
	return physical_pages * PAGE_SIZE;
}

void PhysicalMemory::free_pages(uintptr_t page_number, unsigned count)
{
	set_free_pages(page_number, count);
}

void PhysicalMemory::set_free_pages(uintptr_t page_number, unsigned count)
{
	uintptr_t current_page = page_number;
	for (size_t i = 0; i < count; i++) {
		physical_memory_tracer[current_page / 8] &= ~(1 << (current_page % 8));
		current_page++;
	}
	physical_memory_size -= count * PAGE_SIZE;
}

void PhysicalMemory::set_used_pages(uintptr_t page_number, unsigned count)
{
	uintptr_t current_page = page_number;
	for (size_t i = 0; i < count; i++) {
		physical_memory_tracer[current_page / 8] |= 1 << (current_page % 8);
		current_page++;
	}
	physical_memory_size += count * PAGE_SIZE;
}

uintptr_t PhysicalMemory::find_pages(uintptr_t start_page, size_t count)
{
	// TODO: keep a pointer to the last allocated/freed page to improve the performance.
	uintptr_t first_free_page = 0;
	unsigned remaining_pages = count;
	for (size_t i = start_page / 8; i < sizeof(physical_memory_tracer); i++) {
		if ((physical_memory_tracer[i] == 0xFF) && (remaining_pages != count)) {
			remaining_pages = count;
		} else {
			for (size_t bit = 0; bit < 8; bit++) {
				if (!TEST_BIT(physical_memory_tracer[i], bit)) {
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
	ASSERT_NOT_REACHABLE();
	return uintptr_t(nullptr);
}

uintptr_t PhysicalMemory::get_physical_memory_size()
{
	return physical_memory_size;
}
