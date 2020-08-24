#include "kernel_init.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/idt.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/Console/console.h"
#include "Devices/DebugPort/DebugPort.h"
#include "Devices/DebugPort/Logger.h"
#include "Devices/DeviceFS.h"
#include "Devices/Keyboard/Keyboard.h"
#include "Devices/RTC/rtc.h"
#include "Devices/Timer/pit.h"
#include "Filesystem/pipes/PipeFS.h"
#include "Filesystem/ustar/TarFS.h"
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
		// printf("Time is %d:%d:%d  %d-%d-20%d\n", date.year, date.minutes, date.seconds, date.day_of_month,
		// date.month,
		//      date.year);
		// Scheduler::sleep(500);
		// removeLine();
	}
}

extern "C" void kernel_init(BootloaderInfo* boot_info)
{
	Logger(DebugColor::Bright_Cyan) << "Welcome To CyanOS.\n";

	info() << "Setting up core components... ";
	GDT::setup();
	IDT::setup();
	Memory::setup_stage2();
	Heap::setup();
	Scheduler::setup();
	PIC::setup();
	PIT::setup();
	info() << "Done!\n";

	info() << "Setting up file systems... ";
	VFS::setup();
	VFS::mount(TarFS::alloc(reinterpret_cast<void*>(boot_info->ramdisk.start), boot_info->ramdisk.size));
	VFS::mount(PipeFS::alloc());
	VFS::mount(DeviceFS::alloc());
	info() << "Done!\n";

	info() << "Setting up devices... ";
	DeviceFS::add_device(Keyboard::alloc());
	DeviceFS::add_device(Console::alloc());
	info() << "Done!\n";

	info() << "Starting the first process.\n";
	Process& proc = Process::create_new_process("test_process", "/Tar/Drivers/open_file.exe");
	Thread::create_thread(proc, test_console, 0);
	Thread::create_thread(proc, test_keyboard, 0);

	Thread::yield();
	while (1) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}