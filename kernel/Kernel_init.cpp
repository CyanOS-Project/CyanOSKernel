#include "Kernel_init.h"
#include "Arch/x86/Asm.h"
#include "Arch/x86/Gdt.h"
#include "Arch/x86/Idt.h"
#include "Arch/x86/Paging.h"
#include "Arch/x86/Panic.h"
#include "Arch/x86/Pic.h"
#include "Devices/Console/Console.h"
#include "Devices/DebugPort/DebugPort.h"
#include "Devices/DebugPort/Logger.h"
#include "Devices/DeviceFS.h"
#include "Devices/Keyboard/Keyboard.h"
#include "Devices/RTC/Rtc.h"
#include "Devices/Timer/Pit.h"
#include "Filesystem/Pipes/PipeFS.h"
#include "Filesystem/Socket/SocketFS.h"
#include "Filesystem/Ustar/TarFS.h"
#include "Tasking/Process.h"
#include "Tasking/Scheduler.h"
#include "Tasking/Semaphore.h"
#include "Tasking/Thread.h"
#include "Tests.h"
#include "Utils/Algorithms.h"
#include "Utils/Assert.h"
#include "Utils/UniquePointer.h"
#include "VirtualMemory/Heap.h"
#include "VirtualMemory/Memory.h"

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
	Logger::init();
	Logger(DebugColor::Bright_Cyan) << "Welcome To CyanOS.";

	info() << "Setting up core components... ";
	GDT::setup();
	IDT::setup();
	Memory::setup_stage2();
	Heap::setup();
	Scheduler::setup();
	PIC::setup();
	PIT::setup();
	info() << "\bDone!";

	info() << "Setting up file systems... ";
	VFS::setup();
	VFS::mount(TarFS::alloc("Tar", reinterpret_cast<void*>(boot_info->ramdisk.start), boot_info->ramdisk.size));
	VFS::mount(PipeFS::alloc("Pipes"));
	VFS::mount(DeviceFS::alloc("Devices"));
	VFS::mount(SocketFS::alloc("Sockets"));
	info() << "\bDone!";

	info() << "Setting up devices... ";
	DeviceFS::add_device(Keyboard::alloc("keyboard"));
	DeviceFS::add_device(Console::alloc("console"));
	info() << "\bDone!";

	info() << "Starting the first process.";
	Process& proc = Process::create_new_process("test_process", "/Tar/UserBinary/Shell.exe");
	// Thread::create_thread(proc, test_server, 0);
	// Thread::create_thread(proc, test_client, 0);
	Thread::create_thread(proc, test_ls, 0);

	Thread::yield();
	while (1) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}