#pragma once

#include "arch/x86/isr.h"
#include "arch/x86/paging.h"
#include "arch/x86/panic.h"
#include "arch/x86/spinlock.h"
#include "memory_definitions.h"
#include "physical.h"
#include "virtual.h"
#include <assert.h>
#include <types.h>
#define AVAILABLE_PAGES_START (GET_FRAME(0x100000))

class Memory
{
  public:
	struct ContagiousMemoryBlock {
		void* virtual_addr;
		uintptr_t physical_addr;
		size_t size;
	};

	static unsigned get_kernel_pages();
	static void setup();
	static void setup_page_fault_handler();
	static void* alloc(void* virtual_address, u32 size, u32 flags);
	static void free(void* virtual_address, u32 size, u32 flags);
	static void* map(uintptr_t physical_address, u32 size, u32 flags);
	static void unmap(void* virtual_address, u32 size, u32 flags);
	static void switch_page_directory(uintptr_t physical_address);
	static uintptr_t create_new_virtual_space();
	static unsigned virtual_memory_size();
	static unsigned physical_memory_size();

  private:
	static StaticSpinlock lock;
	static void page_fault_handler(ISRContextFrame& isr_info);
	static u32 parse_flags(u32 mem_flags);
	static void* _alloc_no_lock(void* virtual_address, u32 size, u32 flags);
	static void _free_no_lock(void* virtual_address, u32 size, u32 flags);
	static void* _map_no_lock(uintptr_t physical_address, u32 size, u32 flags);
	static void _unmap_no_lock(void* virtual_address, u32 size, u32 flags);

	friend class VirtualMemory;
};

void* valloc(void* virtual_address, u32 size, u32 flags);
void vfree(void* virtual_address, u32 size, u32 flags);
uintptr_t virtual_to_physical_address(void* virtual_address);