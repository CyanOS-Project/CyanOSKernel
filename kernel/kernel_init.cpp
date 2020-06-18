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
#include "Tasking/scheduler.h"
#include "Tasking/semaphore.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "VirtualMemory/virtual.h"
#include "utils/assert.h"

void display_time()
{
	while (1) {
		DATE_TIME date;
		RTC::get_time(&date);
		printf("Time is %d:%d:%d  %d-%d-20%d\n", date.year, date.minutes, date.seconds, date.day_of_month, date.month,
		       date.year);
		Scheduler::sleep(500);
		removeLine();
	}
}
Semaphore lock;
void thread2()
{
	printf("Thread2:\n");
	semaphore_acquire(&lock);
	printf("Semaphore acquired by thread2\n");
	Scheduler::sleep(1000);
	semaphore_release(&lock);
	printf("Semaphore released by thread2\n");
	while (1) {
		HLT();
	}
}

void thread1()
{
	printf("Thread1:\n");
	semaphore_acquire(&lock);
	Scheduler::create_new_thread((uint32_t)thread2);
	printf("Semaphore acquired by thread1\n");
	Scheduler::sleep(500);
	semaphore_release(&lock);
	printf("Semaphore released by thread1\n");
	while (1) {
		HLT();
	}
}

extern "C" void kernel_init()
{
	initiate_console();
	GDT::setup();
	IDT::setup();
	Memory::setup_page_fault_handler();
	printStatus("Setting up core components.", true);
	Heap::setup();
	Scheduler::setup();
	Scheduler::create_new_thread((uintptr_t)thread1);
	PIC::setup();
	PIT::setup();
	printStatus("Setting up devices.", true);
	printf("Welcome to CyanOS.\n");
	semaphore_init(&lock);
	ENABLE_INTERRUPTS();
	printf("Going Idle State.\n");
	while (1) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}