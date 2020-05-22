#include "include/kernel_init.h"

void kernel_init(){
    clearScreen();
	printf("Welcome to CyanOS");
	asm("hlt");
}