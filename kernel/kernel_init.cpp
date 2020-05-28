#include "kernel_init.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"

void kernel_init()
{
	initiate_console();
	setup_paging();
	setup_gdt();
	setup_idt();
	asm("int3");
	//
	printf("Welcome to CyanOS");
	asm("hlt");
}