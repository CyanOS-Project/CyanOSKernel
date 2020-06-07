#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/RTC/rtc.h"
#include "Devices/Timer/pit.h"
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
	setup_pic();
	setup_pit();
	ENABLE_INTERRUPTS();

	printf("Welcome to CyanOS.\n");
	display_time();
	while (1) {
		HLT();
	}
}