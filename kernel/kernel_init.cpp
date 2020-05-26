#include "kernel_init.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"

void kernel_init()
{
	initiate_console();
	setup_gdt();
	setup_idt();

	asm("int3");

	printf("Welcome to CyanOS");
	asm("hlt");
}