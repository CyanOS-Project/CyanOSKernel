#include "SystemCall.h"
#include "Arch/x86/Isr.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Tasking/Process.h"
#include "Tasking/Thread.h"
#include "Utils/Stl.h"

#pragma GCC diagnostic ignored "-Wcast-function-type"

void SystemCall::setup()
{
	ISR::register_isr_handler(systemcall_handler, SYSCALL_IRQ);
}

generic_syscall SystemCall::get_syscall_routine(unsigned syscall_num)
{
	if (syscall_num < syscalls_count) {
		return systemcalls_routines[syscall_num];
	}
	return nullptr;
}

void SystemCall::systemcall_handler(ISRContextFrame* frame)
{

	dbg() << "System Call " << Context::syscall_num(frame) << " (" << Hex(Context::syscall_param1(frame)) << " ,"
	      << Hex(Context::syscall_param2(frame)) << " ," << Hex(Context::syscall_param3(frame)) << " ,"
	      << Hex(Context::syscall_param4(frame)) << " ," << Hex(Context::syscall_param5(frame)) << ")";

	generic_syscall syscall = get_syscall_routine(Context::syscall_num(frame));
	if (!syscall) {
		PANIC("undefined systemcall invoked");
	}
	auto ret = syscall(Context::syscall_param1(frame), Context::syscall_param2(frame), Context::syscall_param3(frame),
	                   Context::syscall_param4(frame), Context::syscall_param5(frame));

	Context::set_return_value(frame, ret.error());
	if (ret.is_error()) {
		Context::set_return_arg1(frame, 0);
	} else {
		Context::set_return_arg1(frame, ret.value());
	}
}

generic_syscall SystemCall::systemcalls_routines[] = {reinterpret_cast<generic_syscall>(OpenFile),
                                                      reinterpret_cast<generic_syscall>(ReadFile),
                                                      reinterpret_cast<generic_syscall>(WriteFile),
                                                      reinterpret_cast<generic_syscall>(CloseFile),

                                                      reinterpret_cast<generic_syscall>(CreateThread),
                                                      reinterpret_cast<generic_syscall>(CreateRemoteThread), //
                                                      reinterpret_cast<generic_syscall>(CreateProcess),
                                                      reinterpret_cast<generic_syscall>(Sleep),
                                                      reinterpret_cast<generic_syscall>(Yield)};

unsigned SystemCall::syscalls_count = sizeof(systemcalls_routines) / sizeof(generic_syscall);

Result<int> OpenFile(char* path, int mode, int flags)
{
	auto file_description = VFS::open(path, static_cast<OpenMode>(mode), static_cast<OpenFlags>(flags));
	if (file_description.is_error()) {
		return ResultError(file_description.error());
	}

	unsigned fd = Thread::current->parent_process().m_file_descriptors.add_descriptor(move(file_description.value()));
	return fd;
}

Result<int> ReadFile(unsigned descriptor, void* buff, size_t size)
{
	// FIXME: check if descriptor exists.
	auto& description = Thread::current->parent_process().m_file_descriptors.get_description(descriptor);
	auto result = description.read(buff, size);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return 0;
}

Result<int> WriteFile(unsigned descriptor, void* buff, size_t size)
{
	// FIXME: check if descriptor exists.
	auto& description = Thread::current->parent_process().m_file_descriptors.get_description(descriptor);
	auto result = description.read(buff, size);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return 0;
}

Result<int> CloseFile(unsigned descriptor)
{
	// FIXME: check if descriptor exists.
	Thread::current->parent_process().m_file_descriptors.remove_descriptor(descriptor);
	// Destructing the description will call close on FSNode.
	return 0;
}

Result<int> CreateThread(void* address, int arg)
{
	UNUSED(address);
	UNUSED(arg);
	return 0;
}

Result<int> CreateRemoteThread(int process, void* address, int arg)
{
	UNUSED(process);
	UNUSED(address);
	UNUSED(arg);
	return 0;
}

Result<int> CreateProcess(char* name, char* path, int flags)
{
	UNUSED(flags);
	Process::create_new_process(name, path);
	return 0;
}

Result<int> Sleep(size_t size)
{
	Thread::sleep(size);
	return 0;
}

Result<int> Yield()
{
	Thread::yield();
	return 0;
}
