#pragma once

#include "Arch/x86/Isr.h"
#include "Arch/x86/Paging.h"
#include "Arch/x86/Panic.h"
#include "Arch/x86/Spinlock.h"
#include "MemoryDefinitions.h"
#include "Physical.h"
#include "Virtual.h"
#include <Assert.h>
#include <Types.h>
#define AVAILABLE_PAGES_START (GET_FRAME(0x100000))

class Memory
{
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

  private:
	static StaticSpinlock lock;
	static void page_fault_handler(ISRContextFrame& isr_info);
	static uint32_t parse_flags(uint32_t mem_flags);
	static void* _alloc_no_lock(uint32_t size, uint32_t flags);
	static void* _alloc_no_lock(void* virtual_address, uint32_t size, uint32_t flags);
	static void _free_no_lock(void* virtual_address, uint32_t size, uint32_t flags);
	static void* _map_no_lock(uintptr_t physical_address, uint32_t size, uint32_t flags);
	static void _unmap_no_lock(void* virtual_address, uint32_t size, uint32_t flags);
	static void setup_page_fault_handler();

	friend class VirtualMemory;
};

void* valloc(void* virtual_address, uint32_t size, uint32_t flags);
void* valloc(uint32_t size, uint32_t flags);
void vfree(void* virtual_address, uint32_t size, uint32_t flags);