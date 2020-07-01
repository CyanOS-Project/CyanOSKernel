#include "memory.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Devices/Console/console.h"
#include "physical.h"
#include "utils/assert.h"
#include "virtual.h"

SpinLock mem_lock;
void Memory::setup()
{
	Paging::setup(get_kernel_pages());
	PhysicalMemory::initialize();
	// Reserve Low 1MB Pages.
	PhysicalMemory::set_used_pages(0, GET_FRAME(KERNEL_PHYSICAL_ADDRESS));
	// Set kernel memory as used.
	PhysicalMemory::set_used_pages(GET_FRAME(KERNEL_PHYSICAL_ADDRESS), get_kernel_pages());
	spinlock_init(&mem_lock);
}

void Memory::setup_stage2()
{
	setup_page_fault_handler();
}

void Memory::setup_page_fault_handler()
{
	ISR::register_isr_handler(page_fault_handler, IRQ_NUMBER::PF);
}

void Memory::page_fault_handler(ISRContextFrame* isr_info)
{
	printf("Page= %X EIP=%X\t CS=%X\t ESP=%X \t\n", get_faulted_page(), isr_info->eip, isr_info->cs,
	       isr_info->registers.esp);
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
	spinlock_acquire(&mem_lock);
	void* vAdd = _alloc_no_lock(size, flags);
	spinlock_release(&mem_lock);
	return vAdd;
}

void* Memory::alloc(void* virtual_address, uint32_t size, uint32_t flags)
{
	spinlock_acquire(&mem_lock);
	void* vAdd = _alloc_no_lock(virtual_address, size, flags);
	spinlock_release(&mem_lock);
	return vAdd;
}

void Memory::free(void* virtual_address, uint32_t size, uint32_t flags)
{
	spinlock_acquire(&mem_lock);
	_free_no_lock(virtual_address, size, flags);
	spinlock_release(&mem_lock);
}

void* Memory::map(intptr_t physical_address, uint32_t size, uint32_t flags)
{
	spinlock_acquire(&mem_lock);
	void* vAdd = _map_no_lock(physical_address, size, flags);
	spinlock_release(&mem_lock);
	return vAdd;
}

void Memory::unmap(void* virtual_address, uint32_t size, uint32_t flags)
{
	spinlock_acquire(&mem_lock);
	_unmap_no_lock(virtual_address, size, flags);
	spinlock_release(&mem_lock);
}

uintptr_t Memory::create_new_virtual_space()
{
	uintptr_t page_directory = (uintptr_t)alloc(sizeof(PAGE_DIRECTORY), MEMORY_TYPE::WRITABLE | MEMORY_TYPE::KERNEL);
	spinlock_acquire(&mem_lock);
	Paging::map_kernel_pd_entries(page_directory);
	Paging::unmap_pages(page_directory,
	                    GET_PAGES(sizeof(PAGE_DIRECTORY))); // unmap pd from virtual memory but keep it in physical
	uintptr_t physical_address = Paging::get_physical_page(page_directory) * PAGE_SIZE;
	spinlock_release(&mem_lock);
	return physical_address;
}

unsigned Memory::virtual_memory_size()
{
	ASSERT_NOT_REACHABLE();
	return 0;
}

unsigned Memory::physical_memory_size()
{
	return PhysicalMemory::get_physical_memory_size();
}

unsigned Memory::get_kernel_pages()
{
	uintptr_t kernel_size = (uintptr_t)&KERNEL_END - KERNEL_VIRTUAL_ADDRESS;
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

void* Memory::_map_no_lock(intptr_t physical_address, uint32_t size, uint32_t flags)
{
	// FIXME: check if physical is used first.
	if (!size)
		return nullptr;

	intptr_t vAdd;
	intptr_t pAdd = GET_PDE_INDEX((intptr_t)physical_address);
	unsigned pages_num = GET_PAGES(size);

	if (flags & MEMORY_TYPE::KERNEL) {
		vAdd = VirtualMemory::find_pages(KERNEL_VIRTUAL_ADDRESS, LAST_PAGE_ADDRESS, pages_num);
	} else {
		vAdd = VirtualMemory::find_pages(FIRST_PAGE_ADDRESS, KERNEL_VIRTUAL_ADDRESS,
		                                 pages_num); // skip first page to detect null pointer
	}

	PhysicalMemory::set_used_pages(pAdd, pages_num);
	VirtualMemory::map_pages(vAdd, physical_address, GET_PAGES(size), parse_flags(flags));
	// FIXME: deal with address and size when the input address is unaligned.
	return (void*)vAdd;
}

void Memory::_unmap_no_lock(void* virtual_address, uint32_t size, uint32_t flags)
{
	_free_no_lock(virtual_address, size, flags);
}

void Memory::_free_no_lock(void* virtual_address, uint32_t size, uint32_t flags)
{
	unsigned pages_num = GET_PAGES(size);
	intptr_t pAdd = Paging::get_physical_page((intptr_t)virtual_address);
	PhysicalMemory::free_pages(pAdd, pages_num);
	Paging::unmap_pages((intptr_t)virtual_address, pages_num);
}

void* Memory::_alloc_no_lock(void* virtual_address, uint32_t size, uint32_t flags)
{
	intptr_t vAdd;
	unsigned pages_num = GET_PAGES(size);
	vAdd = (intptr_t)virtual_address;
	if (!VirtualMemory::check_free_pages(vAdd, pages_num)) {
		return 0;
	}
	for (size_t i = 0; i < pages_num; i++) {
		intptr_t pAdd = PhysicalMemory::alloc_page();
		VirtualMemory::map_pages(vAdd + (PAGE_SIZE * i), pAdd, 1, parse_flags(flags));
	}
	return (void*)vAdd;
}

void* Memory::_alloc_no_lock(uint32_t size, uint32_t flags)
{
	intptr_t vAdd;
	unsigned pages_num = GET_PAGES(size);

	if (flags & MEMORY_TYPE::KERNEL) {
		vAdd = VirtualMemory::find_pages(KERNEL_VIRTUAL_ADDRESS, LAST_PAGE_ADDRESS, pages_num);
	} else {
		vAdd = VirtualMemory::find_pages(FIRST_PAGE_ADDRESS, KERNEL_VIRTUAL_ADDRESS,
		                                 pages_num); // skip first page to detect null pointer
	}

	for (size_t i = 0; i < pages_num; i++) {

		intptr_t pAdd = PhysicalMemory::alloc_page();
		VirtualMemory::map_pages(vAdd + (PAGE_SIZE * i), pAdd, 1, parse_flags(flags));
	}
	return (void*)vAdd;
}