#include "SystemCall.h"
#include "Arch/x86/Isr.h"
#include "Arch/x86/Panic.h"
#include "Devices/DebugPort/Logger.h"
#include "Tasking/Thread.h"
#include "VirtualMemory/Memory.h"
#include <Algorithms.h>

#pragma GCC diagnostic ignored "-Wcast-function-type"

generic_syscall SystemCall::systemcalls_routines[] = {reinterpret_cast<generic_syscall>(OpenFile),
                                                      reinterpret_cast<generic_syscall>(ReadFile),
                                                      reinterpret_cast<generic_syscall>(WriteFile),
                                                      reinterpret_cast<generic_syscall>(QueryDirectory),
                                                      reinterpret_cast<generic_syscall>(CloseHandle),
                                                      reinterpret_cast<generic_syscall>(Sleep),
                                                      reinterpret_cast<generic_syscall>(Yield),
                                                      reinterpret_cast<generic_syscall>(CreateThread), //
                                                      reinterpret_cast<generic_syscall>(CreateProcess),
                                                      reinterpret_cast<generic_syscall>(OpenProcess),
                                                      reinterpret_cast<generic_syscall>(TerminateProcess),
                                                      reinterpret_cast<generic_syscall>(TerminateThread),
                                                      reinterpret_cast<generic_syscall>(WaitSignal),
                                                      reinterpret_cast<generic_syscall>(VirtualAlloc),
                                                      reinterpret_cast<generic_syscall>(VirtualFree),
                                                      reinterpret_cast<generic_syscall>(QueryFileInformation)};

unsigned SystemCall::syscalls_count = sizeof(systemcalls_routines) / sizeof(generic_syscall);

void SystemCall::setup()
{
	ISR::register_software_interrupt_handler(systemcall_handler, SYSCALL_IRQ);
}

generic_syscall SystemCall::get_syscall_routine(unsigned syscall_num)
{
	if (syscall_num < syscalls_count) {
		return systemcalls_routines[syscall_num];
	}
	return nullptr;
}

void SystemCall::systemcall_handler(ISRContextFrame& frame)
{
	/*dbg() << "System Call " << Context::syscall_num(frame) << " (" << Hex(Context::syscall_param1(frame)) << " ,"
	      << Hex(Context::syscall_param2(frame)) << " ," << Hex(Context::syscall_param3(frame)) << " ,"
	      << Hex(Context::syscall_param4(frame)) << " ," << Hex(Context::syscall_param5(frame)) << ")";*/

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

Result<int> OpenFile(const char* path, int mode, int flags)
{
	if (PathView::is_valid(path) == false) {
		return ResultError(ERROR_INVALID_PARAMETER);
	}

	PathView absolute_path = VFS::resolve_path(path);
	if (VFS::check_exitsts(absolute_path) == false) {
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);
	}

	auto file_description =
	    FileDescription::open(absolute_path, static_cast<OpenMode>(mode), static_cast<OpenFlags>(flags));
	if (file_description.is_error()) {
		return ResultError(file_description.error());
	}

	Handle fd = Thread::current->parent_process().handles().add_handle(move(file_description.value()));
	return fd;
}

Result<int> ReadFile(Handle handle, void* buff, size_t size)
{
	if (Thread::current->parent_process().handles().check_handle(handle) != HandleType::FileDescription)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles().get_file_description(handle);
	auto result = description.read(buff, size);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return result.value();
}

Result<int> WriteFile(Handle handle, void* buff, size_t size)
{
	if (Thread::current->parent_process().handles().check_handle(handle) != HandleType::FileDescription)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles().get_file_description(handle);
	auto result = description.write(buff, size);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return result.value();
}

Result<int> QueryDirectory(Handle handle, FileInfo* info)
{
	if (Thread::current->parent_process().handles().check_handle(handle) != HandleType::FileDescription)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles().get_file_description(handle);
	auto result = description.dir_query(*info);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return 0;
}

Result<int> QueryFileInformation(Handle handle, FileInfo* info)
{
	if (!info)
		return ResultError(ERROR_INVALID_PARAMETER);

	if (Thread::current->parent_process().handles().check_handle(handle) != HandleType::FileDescription)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles().get_file_description(handle);
	auto result = description.file_query(*info);
	if (result.is_error()) {
		return ResultError(result.error());
	}
	return 0;
}

Result<int> CloseHandle(Handle handle)
{
	if (Thread::current->parent_process().handles().check_handle(handle) == HandleType::Invalid)
		return ResultError(ERROR_INVALID_HANDLE);

	Thread::current->parent_process().handles().remove_handle(handle);
	// Destructing the description will call close on FSNode.
	return 0;
}

Result<int> CreateThread(Handle process, void* address, int arg)
{
	UNUSED(process);
	UNUSED(address);
	UNUSED(arg);
	return 0;
}

Result<int> CreateProcess(const char* path, const char* argument, int flags)
{
	UNUSED(flags);

	if (PathView::is_valid(path) == false) {
		return ResultError(ERROR_INVALID_PARAMETER);
	}

	PathView absolute_path = VFS::resolve_path(path);
	if (VFS::check_exitsts(absolute_path) == false) {
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);
	}
	auto& process =
	    Process::create_new_process(absolute_path, (argument == nullptr ? "" : argument), ProcessPrivilege::User);

	auto fp = OpenProcess(process.pid(), 0);
	ASSERT(!fp.is_error());

	return fp.value();
}

Result<int> OpenProcess(size_t pid, int access)
{
	auto process_description = ProcessDescription::open(pid, access);
	if (process_description.is_error()) {
		return ResultError(process_description.error());
	}

	Handle fp = Thread::current->parent_process().handles().add_handle(move(process_description.value()));
	return fp;
}

Result<int> TerminateProcess(Handle handle, int status)
{
	if (handle == Handle(-1)) {
		Thread::current->parent_process().terminate(status);
		Thread::yield();
		return 0;
	} else {
		if (Thread::current->parent_process().handles().check_handle(handle) != HandleType::ProcessDescription)
			return ResultError(ERROR_INVALID_HANDLE);

		auto& description = Thread::current->parent_process().handles().get_process_description(handle);
		description.terminate_process(status);
		return 0;
	}
}

Result<int> TerminateThread(Handle handle, int status)
{
	UNUSED(handle);
	UNUSED(status);
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

Result<int> WaitSignal(Handle handle, int signal)
{
	UNUSED(signal);

	if (Thread::current->parent_process().handles().check_handle(handle) != HandleType::ProcessDescription)
		return ResultError(ERROR_INVALID_HANDLE);

	auto& description = Thread::current->parent_process().handles().get_process_description(handle);
	return description.wait_signal();
}

Result<int> VirtualAlloc(void* address, size_t size, int flags)
{
	void* ret_address = nullptr;
	if (address) {
		ret_address = valloc(address, size, flags | PAGE_USER);
	} else {
		ret_address = valloc(0, size, flags | PAGE_USER);
	}
	return reinterpret_cast<int>(ret_address);
}

Result<int> VirtualFree(void* address, size_t size)
{
	// FIXME: check if it's not kernel page.
	Memory::free(address, size, 0);
	return 0;
}

Result<int> QueryProcessList()
{
	return ERROR_ACCESS_DENIED;
}

Result<int> QueryThreadList()
{
	return ERROR_ACCESS_DENIED;
}

Result<int> QuerySystemInformation()
{
	return ERROR_ACCESS_DENIED;
}

Result<int> QueryProcessInformation()
{
	return ERROR_ACCESS_DENIED;
}

Result<int> QueryThreadInformation()
{
	return ERROR_ACCESS_DENIED;
}
