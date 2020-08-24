#pragma once

#include "Devices/DebugPort/Logger.h"
#include "Devices/Timer/Pit.h"
#include "Filesystem/FileDescription.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Tasking/Loader/PE.h"
#include "Tasking/Scheduler.h"
#include "Tasking/Semaphore.h"
#include "Utils/Bitmap.h"
#include "Utils/PathParser.h"
#include "VirtualMemory/Memory.h"

Semaphore* sem_lock;
void test_semaphore_thread2(uintptr_t arg)
{
	UNUSED(arg);
	// printf("Thread2:\n");
	sem_lock->acquire();
	// printf("Semaphore acquired by thread2\n");
	Thread::sleep(1000);
	sem_lock->release();
	// printf("Semaphore released by thread2\n");
	while (1) {
		HLT();
	}
}

void test_semaphore(uintptr_t arg)
{
	UNUSED(arg);
	// printf("Thread1:\n");
	sem_lock = new Semaphore(1);
	sem_lock->acquire();
	Thread::create_thread(Thread::current->parent_process(), test_semaphore_thread2, 0);
	// printf("Semaphore acquired by thread1\n");
	Thread::sleep(3000);
	// printf("wakeup thread1\n");
	sem_lock->release();
	// printf("Semaphore released by thread1\n");
	while (1) {
		HLT();
	}
}

/*void thread_test(uintptr_t arg)
{
    //printf("Thread %d\n", arg);
    for (size_t i = 0; i < 3; i++) {
        //printf("Thread %d:Hello %d\n", arg, i);
    }
}
void test_threading(uintptr_t arg)
{
    //printf("Main thread: creating other threads\n");
    for (size_t i = 0; i < 3; i++) {
        //printf("Main thread: Creating thread%d\n", i);
        Scheduler::create_new_thread(0, thread_test, i);
    }
}*/

void test_pipe1(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = VFS::open("/fs/my_pipe2", OpenMode::Read, OpenFlags::CreateNew);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error() << '\n';
		HLT();

		return;
	}
	char* buff = (char*)Memory::alloc(0xc00, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	memset(buff, 0, 4096);
	auto result = fd.value()->read(buff, 12);
	dbg() << "got it, read\n";
	dbg() << buff << '\n';
	if (result.is_error())
		warn() << "error reading the file " << result.error() << '\n';
}

void test_pipe2(uintptr_t arg)
{
	UNUSED(arg);
	Thread::sleep(1000);
	auto fd = VFS::open("/fs/my_pipe", OpenMode::Write, OpenFlags::OpenExisting);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error() << '\n';
		HLT();
		return;
	}
	char* buff = (char*)Memory::alloc(0xc00, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	memset(buff, 0, 4096);
	auto result = fd.value()->write(static_cast<const void*>("Hello there"), 12);
	dbg() << "got it, write\n";
	dbg() << buff << '\n';
	if (result.is_error())
		warn() << "error writing the file " << result.error() << '\n';
}

void test_keyboard(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = VFS::open("/Devices/keyboard", OpenMode::ReadWrite, OpenFlags::OpenExisting);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error() << '\n';
		HLT();
		return;
	}
	char buff[1];
	while (true) {
		auto result = fd.value()->read(buff, 1);
		DebugPort::write(buff, DebugColor::Cyan);
	}
}

void test_console(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = VFS::open("/Devices/console", OpenMode::Write, OpenFlags::OpenExisting);
	if (fd.is_error()) {
		// printf("error opening the file, error: %d\n", fd.error());
		HLT();
		return;
	}
	auto result = fd.value()->write("Hello there", 12);
}