#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/RTC/rtc.h"
#include "Devices/Timer/pit.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "VirtualMemory/virtual.h"

void display_time()
{
	while (1) {
		DATE_TIME date;
		get_time(&date);
		printf("Time is %d:%d:%d  %d-%d-20%d\n", date.year, date.minutes, date.seconds, date.day_of_month, date.month,
		       date.year);
		sleep(1000);
		removeLine();
	}
}
extern "C" void kernel_init()
{
	initiate_console();
	printStatus("Setting up paging.", true);
	printStatus("Setting up console.", true);
	GDT::setup();
	printStatus("Setting up GDT.", true);
	setup_idt();
	printStatus("Setting up IDT.", true);
	setup_page_fault_handler();
	printf("Welcome to CyanOS.\n");

	setup_heap();
	volatile intptr_t p1 = kmalloc(0xF00, 0);
	volatile intptr_t p2 = kmalloc(0x10, 0);
	volatile intptr_t p3 = kmalloc(0xA0, 0);
	kfree(p1);
	volatile intptr_t p4 = kmalloc(0xF10, 0);
	volatile intptr_t p5 = kmalloc(10, 0);
	/*setup_pic();
	setup_pit();
	ENABLE_INTERRUPTS();
	printf("Welcome to CyanOS.\n");
	display_time();*/
	while (1) {
		HLT();
	}
}