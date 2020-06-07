#include "virtual.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"

uintptr_t virtual_find_pages(uint32_t start_page, uint32_t end_page, uint32_t count)
{
	size_t remaining_pages = count;
	size_t lookup_start = start_page;
	size_t lookup_end = end_page;
	size_t first_free_page = 0;
	for (size_t i = lookup_start; i < lookup_end; i++) {
		if (!check_page_present(i * PAGE_SIZE)) {
			if (remaining_pages == count)
				first_free_page = i;
			if (!(--remaining_pages))
				return uintptr_t(first_free_page * PAGE_SIZE);
		} else {
			remaining_pages = count;
		}
	}
	PANIC("No virtual memory available!");
}
