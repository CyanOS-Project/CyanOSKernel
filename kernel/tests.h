#pragma once

#include "Devices/Console/console.h"
#include "Devices/Timer/pit.h"
#include "Filesystem/FileDescriptor.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Filesystem/ustar/ustar.h"
#include "Lib/new.h"
#include "Tasking/Loader/pe.h"
#include "Tasking/scheduler.h"
#include "Tasking/semaphore.h"
#include "utils/PathParser.h"
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

	list->push_back({1, 2});
	list->push_back({2, 4});
	list->push_back({1, 2});
	list->push_back({1, 2});
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
	VFS::mount_root(tar_fs->get_root_node());
	auto fd = VFS::open("/Drivers/file2.txt", 0, 0);
	if (fd.is_error()) {
		printf("error opening the file %d\n", fd.error());
		return;
	}

	char* buff = new char[4000];
	memset(buff, 0, 0x100);
	auto result = fd.value().read(buff, 13);
	if (result.is_error())
		printf("error reading the file %d\n", result.error());
	auto loader_result = PELoader::load(buff, 4000);
	if (loader_result.is_error())
		printf("error reading the file %d\n", loader_result.error());

	delete[] buff;
}
