#include "Virtual.h"

uintptr_t VirtualMemory::find_pages(u32 start_address, u32 end_address, u32 pages_num)
{

	u32 remaining_pages = pages_num;
	u32 first_free_page = 0;
	u32 vAdd = start_address;

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
	ASSERT_NOT_REACHABLE();
	return uintptr_t(nullptr);
}

bool VirtualMemory::check_free_pages(u32 start_address, u32 pages_num)
{

	for (size_t i = 0; i < pages_num; i++) {
		if (Paging::check_page_present(start_address + (i * PAGE_SIZE))) {
			return false;
		}
	}
	return true;
}

uintptr_t VirtualMemory::create_page_table()
{
	uintptr_t pt_addr = (uintptr_t)Memory::_alloc_no_lock(0, sizeof(PAGE_TABLE), PAGE_READWRITE);
	Paging::unmap_pages(pt_addr, GET_PAGES(sizeof(PAGE_TABLE))); // we don't need table to be in virtual memory.
	return pt_addr;
}
void VirtualMemory::map_pages(uintptr_t virtual_address, uintptr_t physical_address, u32 pages, u32 flags)
{
	for (size_t i = 0; i < pages; i++) {
		if (Paging::check_page_table_exists(virtual_address) == false) {
			Paging::map_page_table(virtual_address, create_page_table());
		}
		Paging::map_page(virtual_address + PAGE_SIZE * i, physical_address + PAGE_SIZE * i, flags);
	}
}