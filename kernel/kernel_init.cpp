#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "VirtualMemory/virtual.h"

extern "C" void kernel_init()
{
	initiate_console();
	// setup_virtual_memory();
	setup_gdt();
	setup_idt();
	// asm("int3");
	printf("Welcome to CyanOS.\n");
	HLT();
}