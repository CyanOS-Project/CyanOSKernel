#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/Console/console.h"
#include "Devices/RTC/rtc.h"
#include "Devices/Timer/pit.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "VirtualMemory/virtual.h"

void display_time()
{
	while (1) {
		DATE_TIME date;
		RTC::get_time(&date);
		printf("Time is %d:%d:%d  %d-%d-20%d\n", date.year, date.minutes, date.seconds, date.day_of_month, date.month,
		       date.year);
		PIT::sleep(1000);
		removeLine();
	}
}
extern "C" void kernel_init()
{
	initiate_console();
	printStatus("Setting up Console.", true);
	GDT::setup();
	printStatus("Setting up GDT.", true);
	IDT::setup();
	Memory::setup_page_fault_handler();
	printStatus("Setting up IDT.", true);
	PIC::setup();
	printStatus("Setting up PIC.", true);
	PIT::setup();
	printStatus("Setting up PIT.", true);
	Heap::setup();
	printStatus("Setting up Heap.", true);
	printf("Welcome to CyanOS.\n");

	volatile intptr_t p1 = Heap::kmalloc(0xF00, 0);
	volatile intptr_t p2 = Heap::kmalloc(0x10, 0);
	volatile intptr_t p3 = Heap::kmalloc(0xA0, 0);
	Heap::kfree(p1);
	volatile intptr_t p4 = Heap::kmalloc(0xF10, 0);
	volatile intptr_t p5 = Heap::kmalloc(10, 0);

	ENABLE_INTERRUPTS();
	display_time();
	while (1) {
		HLT();
	}
}