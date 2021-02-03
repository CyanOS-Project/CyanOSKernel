#include "Memory.h"
#include "Tasking/ScopedLock.h"

// TODO: Refactor this mess before somone sees it!

StaticSpinlock Memory::lock;
void Memory::setup()
{
	lock.init();
	Paging::setup(get_kernel_pages());
	PhysicalMemory::setup();
	PhysicalMemory::set_used_pages(GET_FRAME(KERNEL_PHYSICAL_ADDRESS), get_kernel_pages());
}

void Memory::setup_page_fault_handler()
{
	ISR::register_isr_handler(page_fault_handler, IRQ_Number::PF);
}

void dump_memory(void* addr, size_t size)
{
	uintptr_t* mem = static_cast<uintptr_t*>(addr);
	err printer;
	for (size_t i = 0; i < size / 4; i++) {
		if (i % 4 == 0) {
			printer << "\n" << &mem[i] << ": ";
		}
		printer << Hex(mem[i]) << "   ";
	}
}
void Memory::page_fault_handler(ISRContextFrame& isr_info)
{
	err() << "--------------------";
	err() << "Page Fault:";
	err() << "Instruction: " << Hex(isr_info.eip);
	err() << "Addr: " << Hex(get_faulted_page());

	if (!PF_PRESENT(isr_info.error_code)) {
		err() << "accessing non-present page";
	}
	if (PF_US(isr_info.error_code)) {
		err() << "accessing kernel page from user mode";
	}
	if (PF_WR(isr_info.error_code)) {
		err() << "writing to read only page";
	}
	dump_memory(reinterpret_cast<void*>(isr_info.registers.esp), 0x20);

	PANIC("");
}

void* Memory::alloc(void* virtual_address, uint32_t size, uint32_t flags)
{
	ScopedLock local_lock(lock);
	void* vAdd = _alloc_no_lock(virtual_address, size, flags);
	return vAdd;
}

void Memory::free(void* virtual_address, uint32_t size, uint32_t flags)
{
	ScopedLock local_lock(lock);
	_free_no_lock(virtual_address, size, flags);
}

void* Memory::map(uintptr_t physical_address, uint32_t size, uint32_t flags)
{
	ScopedLock local_lock(lock);
	void* vAdd = _map_no_lock(physical_address, size, flags);
	return vAdd;
}

void Memory::unmap(void* virtual_address, uint32_t size, uint32_t flags)
{
	ScopedLock local_lock(lock);
	_unmap_no_lock(virtual_address, size, flags);
}

uintptr_t Memory::create_new_virtual_space()
{
	uintptr_t page_directory = (uintptr_t)alloc(0, sizeof(PAGE_DIRECTORY), PAGE_READWRITE);
	ScopedLock local_lock(lock);
	Paging::map_kernel_pd_entries(page_directory);
	// unmap page directory from virtual memory but keep it in physical
	Paging::unmap_pages(page_directory, GET_PAGES(sizeof(PAGE_DIRECTORY)));
	uintptr_t physical_address = Paging::get_physical_page(page_directory) * PAGE_SIZE;
	return physical_address;
}

unsigned Memory::virtual_memory_size()
{
	ScopedLock local_lock(lock);
	ASSERT_NOT_REACHABLE();
	return 0;
}

unsigned Memory::physical_memory_size()
{
	ScopedLock local_lock(lock);
	return PhysicalMemory::get_physical_memory_size();
}

unsigned Memory::get_kernel_pages()
{
	uintptr_t kernel_size = uintptr_t(&KERNEL_END) - uintptr_t(&KERNEL_START) - 1;
	return GET_PAGES(kernel_size);
}

uint32_t Memory::parse_flags(uint32_t mem_flags)
{
	uint32_t page_flags = PAGE_FLAGS_PRESENT;
	if (mem_flags & MEMORY_TYPE::PAGE_READWRITE) {
		page_flags |= PAGE_FLAGS_WRITABLE;
	}
	if (mem_flags & MEMORY_TYPE::PAGE_USER) {
		page_flags |= PAGE_FLAGS_USER;
	}
	return page_flags;
}
void Memory::switch_page_directory(uintptr_t physical_address)
{
	Paging::load_page_directory(physical_address);
}

void* Memory::_map_no_lock(uintptr_t physical_address, uint32_t size, uint32_t flags)
{
	uintptr_t vAdd;
	uintptr_t padding = physical_address % PAGE_SIZE;
	uintptr_t aligned_physical_address = physical_address - padding;
	uintptr_t pAdd = GET_FRAME((uintptr_t)aligned_physical_address);
	size_t pages_num = GET_PAGES(size);

	if (padding)
		pages_num++;

	if (!size) {
		return nullptr;
	}

	if (!PhysicalMemory::check_free_pages(pAdd, pages_num))
		return nullptr;

	if (flags & MEMORY_TYPE::PAGE_USER) {
		vAdd = VirtualMemory::find_pages(FIRST_PAGE_ADDRESS, KERNEL_VIRTUAL_ADDRESS,
		                                 pages_num); // skip first page to detect null pointer
	} else {
		vAdd = VirtualMemory::find_pages(KERNEL_VIRTUAL_ADDRESS, LAST_PAGE_ADDRESS, pages_num);
	}

	PhysicalMemory::set_used_pages(pAdd, pages_num);
	VirtualMemory::map_pages(vAdd, aligned_physical_address, GET_PAGES(size), parse_flags(flags));
	return (void*)(vAdd + padding);
}

void Memory::_unmap_no_lock(void* virtual_address, uint32_t size, uint32_t flags)
{
	_free_no_lock(virtual_address, size, flags);
}

void Memory::_free_no_lock(void* virtual_address, uint32_t size, uint32_t flags)
{
	UNUSED(flags);
	unsigned pages_num = GET_PAGES(size);
	uintptr_t pAdd = Paging::get_physical_page((uintptr_t)virtual_address);
	PhysicalMemory::free_pages(pAdd, pages_num);
	Paging::unmap_pages((uintptr_t)virtual_address, pages_num);
}

void* Memory::_alloc_no_lock(void* virtual_address, uint32_t size, uint32_t flags)
{
	// FIXME: check is address is not kernel page when you are in user mode.
	uintptr_t vAdd;
	unsigned pages_num = GET_PAGES(size);
	vAdd = (uintptr_t)virtual_address;

	if (virtual_address) {
		if (!VirtualMemory::check_free_pages(vAdd, pages_num)) {
			return nullptr;
		}
	} else {
		if (flags & MEMORY_TYPE::PAGE_USER) {
			vAdd = VirtualMemory::find_pages(FIRST_PAGE_ADDRESS, KERNEL_VIRTUAL_ADDRESS,
			                                 pages_num); // skip first page to detect null pointer
		} else {
			vAdd = VirtualMemory::find_pages(KERNEL_VIRTUAL_ADDRESS, LAST_PAGE_ADDRESS, pages_num);
		}
	}

	if (flags & PAGE_CONTAGIOUS) {
		uintptr_t pAdd = PhysicalMemory::alloc_contagious_pages(AVAILABLE_PAGES_START, pages_num);
		VirtualMemory::map_pages(vAdd, pAdd, pages_num, parse_flags(flags));
	} else {
		for (size_t i = 0; i < pages_num; i++) {
			uintptr_t pAdd = PhysicalMemory::alloc_page(AVAILABLE_PAGES_START);
			VirtualMemory::map_pages(vAdd + (PAGE_SIZE * i), pAdd, 1, parse_flags(flags));
		}
	}

	return (void*)vAdd;
}

void* valloc(void* virtual_address, uint32_t size, uint32_t flags)
{
	return Memory::alloc(virtual_address, size, flags);
}

void vfree(void* virtual_address, uint32_t size, uint32_t flags)
{
	return Memory::free(virtual_address, size, flags);
}

uintptr_t virtual_to_physical_address(void* virtual_address)
{
	return Paging::get_physical_page(uintptr_t(virtual_address)) * PAGE_SIZE;
}