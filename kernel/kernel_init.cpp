#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/Console/console.h"
#include "Devices/DebugPort/DebugPort.h"
#include "Devices/DeviceFS.h"
#include "Devices/Keyboard/Keyboard.h"
#include "Devices/RTC/rtc.h"
#include "Devices/Timer/pit.h"
#include "Filesystem/pipes/Pipe.h"
#include "Filesystem/ustar/ustar.h"
#include "Tasking/Process.h"
#include "Tasking/Thread.h"
#include "Tasking/scheduler.h"
#include "Tasking/semaphore.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "VirtualMemory/virtual.h"
#include "tests.h"
#include "utils/UniquePointer.h"
#include "utils/assert.h"
#include "utils/stl.h"

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
	DebugPort::write("Welcome To CyanOS\n", DebugColor::Bright_Cyan);
	GDT::setup();
	IDT::setup();
	Memory::setup_stage2();
	Heap::setup();
	initiate_console();
	printStatus("Setting up core components.", true);
	Scheduler::setup();
	PIC::setup();
	PIT::setup();
	// file systems
	VFS::setup();
	VFS::mount(TarFS::alloc(reinterpret_cast<void*>(info->ramdisk.start), info->ramdisk.size));
	VFS::mount(Pipe::alloc());
	VFS::mount(DeviceFS::alloc());

	DeviceFS::init();
	DeviceFS::add_device(Keyboard::alloc());

	printStatus("Setting up devices.", true);
	printf("Welcome to CyanOS.\n");
	Process& proc = Process::create_new_process("test_process", "/Tar/Drivers/open_file.exe");
	Thread::create_thread(proc, test_keyboard, 0);

	// Process::create_new_process("test_process2", "/Drivers/nop_loop.exe");

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