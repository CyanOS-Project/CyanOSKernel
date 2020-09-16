#include "Process.h"
#include "Arch/x86/Context.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Loader/PE.h"
#include "ScopedLock.h"
#include "Thread.h"
#include "VirtualMemory/Memory.h"
#include <Assert.h>

List<Process>* Process::processes;
Bitmap* Process::pid_bitmap;
StaticSpinlock Process::global_lock;

void Process::setup()
{
	pid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	processes = new List<Process>;
	global_lock.init();
}

Process& Process::create_virtual_process(const StringView& name, ProcessPrivilege privilege)
{
	ScopedLock local_lock(global_lock);
	auto& pcb = processes->emplace_back(name, "", privilege);
	return pcb;
}

Process& Process::create_new_process(const StringView& name, const StringView& path, ProcessPrivilege privilege)
{
	ScopedLock local_lock(global_lock);
	auto& pcb = processes->emplace_back(name, path, privilege);
	Thread::create_thread(pcb, initiate_process, uintptr_t(&pcb), ThreadPrivilege::Kernel);
	return pcb;
}

Result<Process&> Process::get_process_from_pid(size_t pid)
{
	for (auto&& i : *processes) {
		if (i.pid == pid) {
			return i;
		}
	}
	return ResultError(ERROR_INVALID_PID);
}

Process::Process(const StringView& name, const StringView& path, ProcessPrivilege privilege) :
    m_lock{},
    m_singal_waiting_queue{},
    pid{reserve_pid()},
    name{name},
    path{path},
    privilege_level{privilege},
    page_directory{Memory::create_new_virtual_space()},
    parent{nullptr},
    state{ProcessState::Active},
    handles{},
    threads{}
{
}

Process::~Process() {}

Result<uintptr_t> Process::load_executable(const StringView& path)
{
	ScopedLock local_lock(m_lock);
	auto fd = FileDescription::open(path, OpenMode::OM_READ, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		warn() << "error opening the executable file, error: " << fd.error();
		return ResultError(fd.error());
	}
	auto file_info = fd.value()->fstat();
	// FIXME: fix malloc to use more than 4096 bytes and use UniquePointer here.
	char* buff = static_cast<char*>(Memory::alloc(file_info.value().size, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE));
	memset(buff, 0, file_info.value().size);
	auto result = fd.value()->read(buff, file_info.value().size);
	if (result.is_error()) {
		return ResultError(result.error());
	}

	auto execable_entrypoint = PELoader::load(buff, file_info.value().size);
	if (execable_entrypoint.is_error()) {
		return ResultError(execable_entrypoint.error());
	}
	Memory::free(buff, file_info.value().size, 0);
	return execable_entrypoint.value();
}

unsigned Process::reserve_pid()
{
	unsigned id = pid_bitmap->find_first_unused();
	pid_bitmap->set_used(id);
	return id;
}

void Process::initiate_process(uintptr_t __pcb)
{
	Process* pcb = reinterpret_cast<Process*>(__pcb);
	auto&& executable_entrypoint = pcb->load_executable(pcb->path);
	if (executable_entrypoint.is_error()) {
		warn() << "couldn't load the process, error: " << executable_entrypoint.error();
		return; // Terminate Process
	}
	// return ResultError(execable_entrypoint.error());

	if (pcb->privilege_level == ProcessPrivilege::User) {
		void* thread_user_stack = Memory::alloc(STACK_SIZE, MEMORY_TYPE::WRITABLE);
		Context::enter_usermode(executable_entrypoint.value(), uintptr_t(thread_user_stack) + STACK_SIZE - 4);
	} else if (pcb->privilege_level == ProcessPrivilege::Kernel) {
		ASSERT_NOT_REACHABLE(); // TODO: kernel process.
	}

	ASSERT_NOT_REACHABLE();
}

void Process::terminate(int status_code)
{
	ScopedLock local_lock(m_lock);

	for (auto&& thread : threads) {
		thread->terminate();
	}

	state = ProcessState::Terminated;
	m_return_status = status_code;
	m_singal_waiting_queue.wake_up_all();
	// FIXME: free Process memory, maybe static function should call this function ?
}

int Process::wait_for_signal()
{
	ScopedLock local_lock(m_lock);
	m_singal_waiting_queue.wait(local_lock);
	return m_return_status;
}