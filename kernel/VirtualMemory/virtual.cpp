#include "virtual.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"

uintptr_t virtual_find_pages(uint32_t start_page, uint32_t end_page, uint32_t num_pages)
{
	size_t remaining_pages = num_pages;
	size_t lookup_start = start_page;
	size_t lookup_end = end_page;
	size_t found_page = 0;
	for (size_t i = lookup_start; i < lookup_end; i++) {
		if (!check_page_present(i * PAGE_4K)) {
			if (!(--remaining_pages))
				return uintptr_t(i * PAGE_4K);
		} else {
			remaining_pages = num_pages;
			found_page = i;
		}
	}
	PANIC("No virtual memory available!");
}
