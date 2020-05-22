#include "kernel_init.h"
#include "arch/x86/gdt.h"

void kernel_init(){
    clearScreen();
	printf("Welcome to CyanOS");
	asm("hlt");
}