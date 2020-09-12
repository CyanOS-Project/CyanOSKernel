#include "SystemCall.h"
#include "Algorithms.h"
#include "Arch/x86/Isr.h"
#include "Arch/x86/Panic.h"
#include "Devices/DebugPort/Logger.h"
#include "Tasking/Thread.h"

//#pragma GCC diagnostic ignored "-Wcast-function-type"

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

	Handle fd = Thread::current->parent_process().handles.add_file_description(move(file_description.value()));
	return fd;
}

Result<int> ReadFile(Handle handle, void* buff, size_t size)
{
	if (Thread::current->parent_process().handles.check_handle(handle) == false)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles.get_file_description(handle);
	auto result = description.read(buff, size);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return result.value();
}

Result<int> WriteFile(Handle handle, void* buff, size_t size)
{
	if (Thread::current->parent_process().handles.check_handle(handle) == false)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles.get_file_description(handle);
	auto result = description.write(buff, size);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return result.value();
}

Result<int> CloseFile(Handle handle)
{
	if (Thread::current->parent_process().handles.check_handle(handle) == false)
		return ResultError(ERROR_INVALID_HANDLE);

	Thread::current->parent_process().handles.remove_handle(handle);
	// Destructing the description will call close on FSNode.
	return 0;
}

Result<int> QueryDirectory(Handle handle, DirectoryInfo* info)
{
	if (Thread::current->parent_process().handles.check_handle(handle) == false)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles.get_file_description(handle);
	auto result = description.dir_query(info);
	if (result.is_error()) {
		return ResultError(result.error());
	}
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
