#include "memory.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "console.h"
#include "physical.h"
#include "virtual.h"

void Memory::setup()
{
	Paging::setup(get_kernel_pages());
	PhysicalMemory::initialize();
	// Reserve Low 1MB Pages.
	PhysicalMemory::set_used_pages(0, GET_FRAME(KERNEL_PHYSICAL_ADDRESS));
	// Set kernel memory as used.
	PhysicalMemory::set_used_pages(GET_FRAME(KERNEL_PHYSICAL_ADDRESS), get_kernel_pages());
}

void Memory::setup_page_fault_handler()
{
	ISR::register_isr_handler(page_fault_handler, IRQ_NUMBER::PF);
}

void Memory::page_fault_handler(ISR_INFO isr_info)
{
	printf("Page= %X EIP=%X\t CS=%X\t ESP=%X  SS=%X\t\n", isr_info.cr2, isr_info.eip, isr_info.cs, isr_info.esp,
	       isr_info.ss);
	if (!PF_PRESENT(isr_info.error_code)) {
		PANIC("Page fault due accessing non-present page.");
	} else if (PF_US(isr_info.error_code)) {
		PANIC("Page fault due accessing kernel page from user mode.");
	} else if (PF_WR(isr_info.error_code)) {
		PANIC("Page fault due writing to read only page.");
	}
}
uintptr_t Memory::alloc(uint32_t size, uint32_t flags)
{
	uint32_t vAdd;
	uint32_t pages_num = GET_PAGES(size, PAGE_SIZE);

	if (flags & MEMORY_TYPE::KERNEL) {
		vAdd = VirtualMemory::find_pages(GET_FRAME(KERNEL_VIRTUAL_ADDRESS),
		                                 NUMBER_OF_PAGE_DIRECOTRY_ENTRIES * NUMBER_OF_PAGE_TABLE_ENTRIES, pages_num);
	} else {
		vAdd = VirtualMemory::find_pages(1, GET_FRAME(KERNEL_VIRTUAL_ADDRESS),
		                                 pages_num); // skip first page to detect null pointer
	}

	for (size_t i = 0; i < pages_num; i++) {

		uint32_t pAdd = PhysicalMemory::alloc_page();
		Paging::map_pages(vAdd + PAGE_SIZE * i, pAdd, 1, parse_flags(flags));
	}
	return vAdd;
}

uintptr_t Memory::free(uintptr_t address, uint32_t size, uint32_t flags)
{
	PhysicalMemory::free_pages(Paging::get_physical_page(address), GET_PAGES(size, PAGE_SIZE));
	Paging::unmap_pages(address, GET_PAGES(size, PAGE_SIZE));
}

uintptr_t Memory::map(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags)
{
	Paging::map_pages(virtual_address, physical_address, GET_PAGES(size, PAGE_SIZE), parse_flags(flags));
}

void Memory::unmap(uint32_t virtual_address, uint32_t physical_address, uint32_t size, uint32_t flags)
{
	Paging::unmap_pages(virtual_address, GET_PAGES(size, PAGE_SIZE));
}

uint32_t Memory::virtual_memory_size()
{
}

uint32_t Memory::physical_memory_size()
{
	return PhysicalMemory::get_physical_memory_size();
}

uint32_t Memory::get_kernel_pages()
{
	uint32_t kernel_size = (uint32_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
	uint32_t pages = kernel_size / PAGE_SIZE + ((kernel_size % PAGE_SIZE == 0) ? 0 : 1);
	return pages;
}

uint32_t Memory::parse_flags(uint32_t mem_flags)
{
	uint32_t page_flags = PAGE_FLAGS_PRESENT;
	if (mem_flags & MEMORY_TYPE::WRITABLE) {
		page_flags |= PAGE_FLAGS_WRITABLE;
	}
	if (!(mem_flags & MEMORY_TYPE::KERNEL)) {
		page_flags |= PAGE_FLAGS_USER;
	}
	return page_flags;
}