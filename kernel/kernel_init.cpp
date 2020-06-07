#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/Timer/pit.h"
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
	setup_page_fault_handler();
	setup_pic();
	setup_pit();
	ENABLE_INTERRUPTS();

	printf("Welcome to CyanOS.\n");
	while (1) {
		HLT();
	}
}