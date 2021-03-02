#include "Kernel_init.h"
#include "Arch/x86/Asm.h"
#include "Arch/x86/Gdt.h"
#include "Arch/x86/Idt.h"
#include "Arch/x86/Paging.h"
#include "Arch/x86/Panic.h"
#include "Arch/x86/Pic.h"
#include "Devices/Bus/PCI.h"
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

extern "C" void kernel_init(BootloaderInfo* boot_info)
{
	Logger(DebugColor::Bright_Cyan) << "Welcome To CyanOS.";

	info() << "Setting up core components... ";
	GDT::setup();
	IDT::setup();
	Memory::setup_page_fault_handler();
	Heap::setup();

	call_constrcutors();
	PIT::setup();
	Scheduler::setup();
	info() << "Done!";

	info() << "Setting up file systems... ";
	VFS::mount(TarFS::alloc("tar", boot_info->ramdisk.start, boot_info->ramdisk.size));
	VFS::mount(PipeFS::alloc("pipes"));
	VFS::mount(DeviceFS::alloc("devices"));
	VFS::mount(SocketFS::alloc("sockets"));
	info() << "Done!";

	info() << "Setting up devices... ";
	DeviceFS::add_device(Keyboard::alloc("keyboard"));
	DeviceFS::add_device(Console::alloc("console"));
	info() << "Done!";

	info() << "Starting the system process...";
	Process& proc = Process::create_virtual_process("system");
	Thread::create_init_thread(proc); // It will just continue here.
	Thread::yield();
	Thread::create_thread(proc, idle, ThreadPrivilege::Kernel);
	info() << "Done!";

	info() << "CyanOS is ready!";

	PCI::enumerate_pci_devices();

	Process::create_new_process("/tar/bin/shell", "", ProcessPrivilege::User);

	ENABLE_INTERRUPTS();
	Thread::yield();
	while (true) {
		HLT();
	}
	ASSERT_NOT_REACHABLE();
}

void idle()
{
	while (true) {
		HLT();
	}
}

void call_constrcutors()
{
	typedef void (*func)(void);
	uintptr_t* constructor_array = &CONSTRUCTORS_ARRAY_START;
	while (constructor_array != &CONSTRUCTORS_ARRAY_END && *constructor_array && *constructor_array != uintptr_t(-1)) {
		func constructor = (func)*constructor_array;
		constructor();
		constructor_array++;
	}
}