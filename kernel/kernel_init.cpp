#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/atomic.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/Console/console.h"
#include "Devices/RTC/rtc.h"
#include "Devices/Timer/pit.h"
#include "Tasking/scheduler.h"
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
void int3irq(ContextFrame* frame)
{
	printf("hello");
}
extern "C" void kernel_init()
{
	initiate_console();
	IDT::setup();
	GDT::setup();
	uintptr_t new_stack = Memory::alloc(0x1000, MEMORY_TYPE::KERNEL);
	GDT::setup_tss(new_stack);
	Memory::setup_page_fault_handler();
	atomic_intiate();
	printStatus("Setting up core components.", true);
	Heap::setup();
	Scheduler::setup();
	PIC::setup();
	PIT::setup();
	printStatus("Setting up devices.", true);
	printf("Welcome to CyanOS.\n");
	ENABLE_INTERRUPTS();
	display_time();
	printf("Going Idle State.\n");
	while (1) {
		HLT();
	}
}