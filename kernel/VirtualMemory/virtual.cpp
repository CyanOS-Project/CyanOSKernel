#include "virtual.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"

uintptr_t VirtualMemory::find_pages(uint32_t start_address, uint32_t end_address, uint32_t pages_num)
{

	uint32_t remaining_pages = pages_num;
	uint32_t first_free_page = 0;
	uint32_t vAdd = start_address;

	while (vAdd < end_address) {
		if (!Paging::check_page_present(vAdd)) {
			if (remaining_pages == pages_num)
				first_free_page = vAdd;
			if (!(--remaining_pages))
				return uintptr_t(first_free_page);
		} else {
			remaining_pages = pages_num;
		}
		vAdd += PAGE_SIZE;
	}

	PANIC("No virtual memory available!");
}

bool VirtualMemory::check_free_pages(uint32_t start_address, uint32_t pages_num)
{

	for (size_t i = 0; i < pages_num; i++) {
		if (Paging::check_page_present(start_address + (i * PAGE_SIZE))) {
			return false;
		}
	}
	return true;
}