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
	printStatus("Setting up paging.", true);
	printStatus("Setting up console.", true);
	setup_gdt();
	printStatus("Setting up GDT.", true);
	setup_idt();
	printStatus("Setting up IDT.", true);
	printf("Welcome to CyanOS.\n");
	HLT();
}