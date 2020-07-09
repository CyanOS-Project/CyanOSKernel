#pragma once

#include "Devices/Console/console.h"
#include "Devices/Timer/pit.h"
#include "Filesystem/ustar/ustar.h"
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
	printf("Thread %d\n", arg);
	for (size_t i = 0; i < 3; i++) {
		printf("Thread %d:Hello %d\n", arg, i);
	}
}
void test_threading(uintptr_t arg)
{
	printf("Main thread: creating other threads\n");
	for (size_t i = 0; i < 3; i++) {
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

void test_tar_filesystem(uintptr_t fs)
{
	printf("tar at %X\n", fs);
	TarFS* tar_fs = new TarFS((void*)fs);
	char* file;

	printf("file1 size : %d\n", tar_fs->get_file_size("Drivers/file1.txt"));
	file = tar_fs->read_file("Drivers/file1.txt");
	if (file)
		printf("Output : %s\n", file);

	printf("file2 size : %d\n", tar_fs->get_file_size("Drivers/file2.txt"));
	file = tar_fs->read_file("Drivers/file2.txt");
	if (file)
		printf("Output : %s\n", file);

	printf("file3 size : %d\n", tar_fs->get_file_size("UserBinary/file3.txt"));
	file = tar_fs->read_file("UserBinary/file3.txt");
	if (file)
		printf("Output : %s\n", file);
}

class ParentClass
{
  private:
	int d;

  public:
	ParentClass(int n)
	{
		d = n;
		printf("creating the parent %d class.\n", d);
	}
	~ParentClass()
	{
		printf("destroying the parent %d class.\n", d);
	}
	virtual void hello() = 0;

	void hello2()
	{
		printf("this is the parent %d class.\n", d);
	}
};

class ChildClass : public ParentClass
{
  private:
	int d;

  public:
	ChildClass(int n) : ParentClass(n)
	{
		d = n;
		printf("creating the child %d class.\n", d);
	}
	~ChildClass()
	{
		printf("destroying the child %d class.\n", d);
	}
	virtual void hello()
	{
		printf("this is the child %d class.\n", d);
	}
	void hello2()
	{
		printf("this is the child %d class.\n", d);
	}
};

void test_virtual_functions()
{
	ParentClass* parent;
	ChildClass child(4);
	parent = &child;
	parent->hello();
	parent->hello2();
}
