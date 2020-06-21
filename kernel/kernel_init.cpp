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
// Semaphore lock;
void thread2()
{
	printf("Thread2:\n");
	/*semaphore_acquire(&lock);
	printf("Semaphore acquired by thread2\n");
	Scheduler::sleep(1000);
	semaphore_release(&lock);
	printf("Semaphore released by thread2\n");*/
	while (1) {
		HLT();
	}
}

void thread1()
{
	printf("Thread1:\n");
	Scheduler::sleep(500);
	printf("Thread1:end\n");
	/*semaphore_init(&lock);
	semaphore_acquire(&lock);
	Scheduler::create_new_thread((uint32_t)thread2);
	printf("Semaphore acquired by thread1\n");
	Scheduler::sleep(500);
	semaphore_release(&lock);
	printf("Semaphore released by thread1\n");*/
	while (1) {
		HLT();
	}
}

typedef struct Stuff_t {
	int value1;
	int value2;
} Stuff;

void test()
{
	CircularList<Stuff>* list = new CircularList<Stuff>;
	CircularList<Stuff>* list2 = new CircularList<Stuff>;

	Stuff s1 = {1, 1};
	Stuff s2 = {2, 2};
	Stuff s3 = {3, 3};
	Stuff s4 = {4, 4};
	list->push_back(s1);
	list->push_back(s2);
	list->push_back(s3);
	list->push_back(s4);
	printf("List 1:\n");
	CircularList<Stuff>::Iterator itr = CircularList<Stuff>::Iterator(list);
	list->move_head_to_other_list(list2);
	list->move_head_to_other_list(list2);
	list->move_head_to_other_list(list2);
	itr.set_cursor(0);
	// list->remove(1);
	do {
		Stuff& cur = list->data(itr);
		printf("%d %d\n", cur.value1, cur.value2);
		itr++;
	} while (!itr.is_head());

	printf("List 2:\n");
	CircularList<Stuff>::Iterator itr2 = CircularList<Stuff>::Iterator(list2);
	// list->remove(0);
	itr2.set_cursor(0);
	s1.value1 = 5;
	do {
		Stuff& cur = list2->data(itr2);
		printf("%d %d\n", cur.value1, cur.value2);
		itr2++;
	} while (!itr2.is_head());
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
	Scheduler::create_new_thread((uintptr_t)thread2);
	PIC::setup();
	PIT::setup();
	printStatus("Setting up devices.", true);
	printf("Welcome to CyanOS.\n");
	ENABLE_INTERRUPTS();
	// test();
	printf("Going Idle State.\n");
	while (1) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}