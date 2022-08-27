#include "kernel_init.h"
#include "arch/x86/asm.h"
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "arch/x86/paging.h"
#include "arch/x86/panic.h"
#include "arch/x86/pic.h"
#include "devices/bus/pci.h"
#include "devices/console/console.h"
#include "devices/debug_port/debug_port.h"
#include "devices/debug_port/logger.h"
#include "devices/device_fs.h"
#include "devices/keyboard/keyboard.h"
#include "devices/rtc/rtc.h"
#include "devices/timer/pit.h"
#include "filesystem/domain_socket/domain_socket_fs.h"
#include "filesystem/pipes/pipe_fs.h"
#include "filesystem/ustar/tar_fs.h"
#include "tasking/process.h"
#include "tasking/scheduler.h"
#include "tasking/semaphore.h"
#include "tasking/thread.h"
#include "tests.h"
#include "virtual_memory/heap.h"
#include "virtual_memory/memory.h"
#include <algorithms.h>
#include <assert.h>
#include <unique_pointer.h>

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
	VFS::mount(DomainSocketFS::alloc("sockets"));
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