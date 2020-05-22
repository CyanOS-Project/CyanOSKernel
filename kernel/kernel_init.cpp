#include "kernel_init.h"

void kernel_init(){
	int dd = 0;
	int d = 45/dd;
    clearScreen();
	printf("Welcome to CyanOS %d",d);
	asm("hlt");
}