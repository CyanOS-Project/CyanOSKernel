#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/Console/console.h"
#include "Devices/DebugPort/DebugPort.h"
#include "Devices/RTC/rtc.h"
#include "Devices/Timer/pit.h"
#include "Filesystem/pipes/Pipe.h"
#include "Tasking/Process.h"
#include "Tasking/Thread.h"
#include "Tasking/scheduler.h"
#include "Tasking/semaphore.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "VirtualMemory/virtual.h"
#include "tests.h"
#include "utils/assert.h"
void display_time()
{
	while (1) {
		DATE_TIME date;
		RTC::get_time(&date);
		printf("Time is %d:%d:%d  %d-%d-20%d\n", date.year, date.minutes, date.seconds, date.day_of_month, date.month,
		       date.year);
		// Scheduler::sleep(500);
		removeLine();
	}
}

extern "C" void kernel_init(BootloaderInfo* info)
{
	DebugPort::write("Welcome To CyanOS", DebugColor::Bright_Cyan);
	GDT::setup();
	IDT::setup();
	Memory::setup_stage2();
	Heap::setup();
	initiate_console();
	printStatus("Setting up core components.", true);
	Scheduler::setup();
	PIC::setup();
	PIT::setup();
	printStatus("Setting up devices.", true);
	printf("Welcome to CyanOS.\n");
	TarFS* tar_fs = new TarFS(reinterpret_cast<void*>(info->ramdisk.start), info->ramdisk.size);
	VFS::setup();
	VFS::mount_root(tar_fs->root_node());
	VFS::mount("/fs", Pipe::root_node());
	Process& proc = Process::create_new_process("test_process", "/Drivers/syscall.exe");
	Thread::create_thread(proc, test_pipe1, 0);
	Thread::create_thread(proc, test_pipe2, 0);

	Thread::yield();
	// **************** tests**************************

	test_tar_filesystem(info->ramdisk.start, info->ramdisk.size);

	// **************** end tests *********************
	printf("Going Idle State.\n");
	while (1) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}