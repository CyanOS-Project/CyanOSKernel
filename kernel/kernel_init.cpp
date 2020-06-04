#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "VirtualMemory/memory.h"
#include "VirtualMemory/virtual.h"

extern "C" void kernel_init()
{
	initiate_console();
	printStatus("Setting up paging.", true);
	printStatus("Setting up console.", true);
	setup_gdt();
	printStatus("Setting up GDT.", true);
	setup_idt();
	printStatus("Setting up IDT.", true);

	volatile uintptr_t dd = memory_alloc(0x3000, MEMORY::KERNEL);
	volatile uintptr_t dd2 = memory_alloc(0x1000, MEMORY::KERNEL);
	volatile uintptr_t dd3 = memory_alloc(0x1000, MEMORY::KERNEL);
	memory_free(dd2, 0x1000, 0);
	volatile uintptr_t dd4 = memory_alloc(0x1000, MEMORY::KERNEL);
	volatile uintptr_t dd5 = memory_alloc(0x1000, MEMORY::KERNEL);
	printf("Welcome to CyanOS.\n");
	HLT();
}