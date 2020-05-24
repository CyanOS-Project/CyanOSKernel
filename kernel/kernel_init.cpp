#include "kernel_init.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"

void kernel_init()
{
	setup_gdt();
	setup_idt();

	asm("int3");
	clearScreen();
	printf("Welcome to CyanOS");
	asm("hlt");
}