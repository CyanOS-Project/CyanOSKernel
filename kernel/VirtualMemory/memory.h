#pragma once

#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/spinlock.h"
#include "Devices/Console/console.h"
#include "physical.h"
#include "utils/assert.h"
#include "utils/types.h"
#include "virtual.h"

enum MEMORY_TYPE {
	KERNEL = 1,
	WRITABLE = 2,
	COPY_ON_WRITE = 4,
};

#define AVAILABLE_PAGES_START (GET_FRAME(0x100000))

class Memory
{
  private:
	static StaticSpinlock lock;
	static void page_fault_handler(ISRContextFrame* isr_info);
	static uint32_t parse_flags(uint32_t mem_flags);
	static void* _alloc_no_lock(uint32_t size, uint32_t flags);
	static void* _alloc_no_lock(void* virtual_address, uint32_t size, uint32_t flags);
	static void _free_no_lock(void* virtual_address, uint32_t size, uint32_t flags);
	static void* _map_no_lock(uintptr_t physical_address, uint32_t size, uint32_t flags);
	static void _unmap_no_lock(void* virtual_address, uint32_t size, uint32_t flags);
	static void setup_page_fault_handler();

  public:
	static unsigned get_kernel_pages();
	static void setup();
	static void setup_stage2();
	static void* alloc(uint32_t size, uint32_t flags);
	static void* alloc(void* virtual_address, uint32_t size, uint32_t flags);
	static void free(void* virtual_address, uint32_t size, uint32_t flags);
	static void* map(uintptr_t physical_address, uint32_t size, uint32_t flags);
	static void unmap(void* virtual_address, uint32_t size, uint32_t flags);
	static void switch_page_directory(uintptr_t physical_address);
	static uintptr_t create_new_virtual_space();
	static unsigned virtual_memory_size();
	static unsigned physical_memory_size();

	friend class VirtualMemory;
};
