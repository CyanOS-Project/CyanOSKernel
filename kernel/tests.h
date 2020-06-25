#pragma once

#include "Devices/Console/console.h"
#include "Devices/Timer/pit.h"
#include "Tasking/scheduler.h"
#include "Tasking/semaphore.h"
#include "utils/bitmap.h"
#include "utils/list.h"

Semaphore* lock;
void test_semaphore_thread2(uintptr_t arg)
{
	printf("Thread2:\n");
	lock->acquire();
	printf("Semaphore acquired by thread2\n");
	Scheduler::sleep(1000);
	lock->release();
	printf("Semaphore released by thread2\n");
	while (1) {
		HLT();
	}
}

void test_semaphore(uintptr_t arg)
{
	printf("Thread1:\n");
	lock = new Semaphore(1);
	lock->acquire();
	Scheduler::create_new_thread(0, test_semaphore_thread2, 0);
	printf("Semaphore acquired by thread1\n");
	Scheduler::sleep(500);
	lock->release();
	printf("Semaphore released by thread1\n");
	while (1) {
		HLT();
	}
}

void thread_test(uintptr_t arg)
{
	volatile int* data = (int*)Memory::alloc(0x200, MEMORY_TYPE::WRITABLE);
	printf("Thread %d\n", arg);
	*data = arg;
	while (1) {
		printf("ending thread %d ; ", arg);
		printStatus("", *data == arg);

		if (*data != arg) {
			printf("");
		}

		Scheduler::sleep(3000);
	}
}
void test_threading(uintptr_t arg)
{
	printf("Main thread: creating other threads\n");
	for (size_t i = 0; i < 5; i++) {
		printf("Main thread: Creating thread%d\n", i);
		Scheduler::create_new_thread(0, thread_test, i);
	}
}

typedef struct Stuff_t {
	int value1;
	int value2;
} Stuff;

void test_lists()
{
	CircularQueue<Stuff>* list = new CircularQueue<Stuff>;
	CircularQueue<Stuff>* list2 = new CircularQueue<Stuff>;

	Stuff s1 = {1, 1};
	Stuff s2 = {2, 2};
	Stuff s3 = {3, 3};
	Stuff s4 = {4, 4};
	list->push_back(s1);
	list->push_back(s2);
	list->push_back(s3);
	list->push_back(s4);
	printf("List 1:\n");
	CircularQueue<Stuff>::Iterator itr = list->begin();
	list->move_head_to_other_list(list2);
	list->move_head_to_other_list(list2);
	// list->move_head_to_other_list(list2);
	// list->move_head_to_other_list(list2);
	list->remove(0);

	for (CircularQueue<Stuff>::Iterator i = list->begin(); i != list->end(); ++i) {
		printf("%d %d\n", *i, *i);
	}
}

void test_bitmap()
{
	Bitmap my_bitmap(10);
	my_bitmap.set_used(2);
	printf("find_first_used %d\n", my_bitmap.find_first_unused(5));
	my_bitmap.set_unused(2);
	printf("find_first_used %d\n", my_bitmap.find_first_unused(5));
	my_bitmap.set_used(8);
	printf("find_first_used %d\n", my_bitmap.find_first_used(5));
	printf("find_first_used %d\n", my_bitmap.find_first_used(1));
}