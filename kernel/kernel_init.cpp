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
#include "tests.h"
#include "utils/UnitTests.h"
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

extern "C" void kernel_init(BootloaderInfo* info)
{
	GDT::setup();
	IDT::setup();
	Memory::setup_stage2();
	initiate_console();
	printStatus("Setting up core components.", true);
	Heap::setup();
	Scheduler::setup();
	auto& new_proc = Scheduler::create_new_process("test_process");
	Scheduler::create_new_thread(&new_proc, test_threading, 0);
	PIC::setup();
	PIT::setup();
	printStatus("Setting up devices.", true);
	printf("Welcome to CyanOS.\n");
	// Scheduler::yield();
	// test_virtual_functions();
	/*uintptr_t ustar_fs = reinterpret_cast<uintptr_t>(
	    Memory::map(info->ramdisk.start, info->ramdisk.size, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE));
	test_tar_filesystem(ustar_fs);*/
	// Scheduler::yield();
	// test_lists();
	// test_bitmap();
	// UnitTest_Bitmap();
	// UnitTest_CircularQueue();
	test_path();
	printf("Going Idle State.\n");
	while (1) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}