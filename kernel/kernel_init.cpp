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
#include "utils/list.h"

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
	semaphore_init(&lock);
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

struct Stuff {
	int value1;
	int value2;
};

void test()
{
	CircularList<Stuff> list = *new CircularList<Stuff>;
	list.push_back({.value1 = 1, .value2 = 2});
	list.push_back({.value1 = 3, .value2 = 4});
	for (size_t i = 0; i < list.count; i++) {
		printf("%d %d\n", list[i].value1, list[i].value2);
	}

	/*CircularList<Stuff>* list = new CircularList<Stuff>({.value1 = 1, .value2 = 2});
	/*for (size_t i = 0; i < 5; i++) {
	    CircularList<Stuff>* new_node = new CircularList<Stuff>({.value1 = 1, .value2 = 2});
	    list->add_node(new_node);
	}
	CircularList<Stuff>* node_iterator = list->begin();
	do {
	    node_iterator->operator++();
	} while (node_iterator != list->begin());


	*/
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
	// ENABLE_INTERRUPTS();
	test();
	printf("Going Idle State.\n");
	while (1) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}