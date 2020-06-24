#include "memory.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Devices/Console/console.h"
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

void Memory::page_fault_handler(ContextFrame* isr_info)
{
	printf("Page= %X EIP=%X\t CS=%X\t ESP=%X \t\n", isr_info->cr2, isr_info->eip, isr_info->cs, isr_info->esp);
	if (!PF_PRESENT(isr_info->error_code)) {
		PANIC("Page fault due accessing non-present page.");
	} else if (PF_US(isr_info->error_code)) {
		PANIC("Page fault due accessing kernel page from user mode.");
	} else if (PF_WR(isr_info->error_code)) {
		PANIC("Page fault due writing to read only page.");
	}
}
void* Memory::alloc(uint32_t size, uint32_t flags)
{
	intptr_t vAdd, pAdd;
	unsigned pages_num = GET_PAGES(size);

	if (flags & MEMORY_TYPE::KERNEL) {
		vAdd = VirtualMemory::find_pages(KERNEL_VIRTUAL_ADDRESS, LAST_PAGE_ADDRESS, pages_num);
	} else {
		vAdd = VirtualMemory::find_pages(FIRST_PAGE_ADDRESS, KERNEL_VIRTUAL_ADDRESS,
		                                 pages_num); // skip first page to detect null pointer
	}

	for (size_t i = 0; i < pages_num; i++) {

		intptr_t pAdd = PhysicalMemory::alloc_page();
		Paging::map_pages(vAdd + (PAGE_SIZE * i), pAdd, 1, parse_flags(flags));
	}
	return (void*)vAdd;
}

void* Memory::alloc(void* virtual_address, uint32_t size, uint32_t flags)
{
	intptr_t vAdd, pAdd;
	unsigned pages_num = GET_PAGES(size);
	vAdd = (intptr_t)virtual_address;
	if (!VirtualMemory::check_free_pages(vAdd, pages_num)) {
		return 0;
	}
	for (size_t i = 0; i < pages_num; i++) {
		intptr_t pAdd = PhysicalMemory::alloc_page();
		Paging::map_pages(vAdd + (PAGE_SIZE * i), pAdd, 1, parse_flags(flags));
	}
	return (void*)vAdd;
}

void Memory::free(void* virtual_address, uint32_t size, uint32_t flags)
{
	unsigned pages_num = GET_PAGES(size);
	PhysicalMemory::free_pages(Paging::get_physical_page((uintptr_t)virtual_address), pages_num);
	Paging::unmap_pages((intptr_t)virtual_address, pages_num);
}

void Memory::map(void* virtual_address, void* physical_address, uint32_t size, uint32_t flags)
{
	Paging::map_pages((intptr_t)virtual_address, (intptr_t)physical_address, GET_PAGES(size), parse_flags(flags));
}

void Memory::unmap(void* virtual_address, void* physical_address, uint32_t size, uint32_t flags)
{
	Paging::unmap_pages((intptr_t)virtual_address, GET_PAGES(size));
}

unsigned Memory::virtual_memory_size()
{
}

unsigned Memory::physical_memory_size()
{
	return PhysicalMemory::get_physical_memory_size();
}

unsigned Memory::get_kernel_pages()
{
	intptr_t kernel_size = (intptr_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
	unsigned pages = kernel_size / PAGE_SIZE + ((kernel_size % PAGE_SIZE == 0) ? 0 : 1);
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
void Memory::switch_page_directory(uintptr_t physical_address)
{
	Paging::load_page_directory(physical_address);
}