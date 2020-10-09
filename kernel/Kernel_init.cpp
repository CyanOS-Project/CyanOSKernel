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
#include "VirtualMemory/Heap.h"
#include "VirtualMemory/Memory.h"
#include <Algorithms.h>
#include <Assert.h>
#include <UniquePointer.h>

void idle(uintptr_t);
extern "C" void kernel_init(BootloaderInfo* boot_info)
{
	Logger::init();
	Logger(DebugColor::Bright_Cyan) << "Welcome To CyanOS.";

	info() << "Setting up core components... ";
	GDT::setup();
	IDT::setup();

	Memory::setup_stage2();
	Heap::setup();
	PIC::setup();
	PIT::setup();
	Scheduler::setup();
	info() << "\bDone!";

	info() << "Setting up file systems... ";
	VFS::setup();
	VFS::mount(TarFS::alloc("tar", reinterpret_cast<void*>(boot_info->ramdisk.start), boot_info->ramdisk.size));
	VFS::mount(PipeFS::alloc("pipes"));
	VFS::mount(DeviceFS::alloc("devices"));
	VFS::mount(SocketFS::alloc("sockets"));
	info() << "\bDone!";

	info() << "Setting up devices... ";
	DeviceFS::add_device(Keyboard::alloc("keyboard"));
	DeviceFS::add_device(Console::alloc("console"));
	info() << "\bDone!";

	PathView ff("/");
	info() << "Starting the system process...";
	Process& proc = Process::create_virtual_process("system", ProcessPrivilege::User);
	Thread::create_init_thread(proc); // It will just continue here.
	Thread::yield();
	Thread::create_thread(proc, idle, 0, ThreadPrivilege::Kernel);
	info() << "\bDone!";

	info() << "CyanOS is ready!";
	Process::create_new_process("/tar/bin/shell", "", ProcessPrivilege::User);

	ENABLE_INTERRUPTS();
	Thread::yield();
	while (true) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}

void idle(uintptr_t)
{
	while (true) {
		HLT();
	}
}