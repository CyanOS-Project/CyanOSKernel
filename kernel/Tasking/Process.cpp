#include "Process.h"
#include "Arch/x86/Context.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Loader/PE.h"
#include "ScopedLock.h"
#include "Thread.h"
#include "VirtualMemory/Memory.h"
#include <Assert.h>

List<Process>* Process::processes;
Bitmap<MAX_BITMAP_SIZE>* Process::pid_bitmap;
StaticSpinlock Process::global_lock;

void Process::setup()
{
	pid_bitmap = new Bitmap<MAX_BITMAP_SIZE>;
	processes = new List<Process>;
	global_lock.init();
}

Process& Process::create_virtual_process(const StringView& name, ProcessPrivilege privilege)
{
	ScopedLock local_lock(global_lock);
	auto& pcb = processes->emplace_back(name, "", privilege);
	return pcb;
}

Process& Process::create_new_process(const StringView& path, const StringView& argument, ProcessPrivilege privilege)
{
	ScopedLock local_lock(global_lock);

	auto& pcb = processes->emplace_back(path, argument, privilege);
	Thread::create_thread(pcb, initiate_process, uintptr_t(&pcb), ThreadPrivilege::Kernel);
	return pcb;
}

Result<Process&> Process::get_process_from_pid(size_t pid)
{
	for (auto&& i : *processes) {
		if (i.m_pid == pid) {
			return i;
		}
	}
	return ResultError(ERROR_INVALID_PID);
}

Process::Process(const StringView& path, const StringView& argument, ProcessPrivilege privilege) :
    m_lock{},
    m_singal_waiting_queue{},
    m_pid{reserve_pid()},
    m_name{PathParser(path).last_element()},
    m_path{path},
    m_argument{argument},
    m_privilege_level{privilege},
    m_page_directory{Memory::create_new_virtual_space()},
    m_parent{nullptr},
    m_state{ProcessState::Active},
    m_handles{},
    m_threads{},
    m_pib{nullptr}
{
	Memory::switch_page_directory(m_page_directory);

	m_pib = reinterpret_cast<UserProcessInformationBlock*>(
	    Memory::alloc(sizeof(UserProcessInformationBlock), MEMORY_TYPE::WRITABLE));

	m_pib->pid = m_pid;
	m_pib->path = m_pib->path_data;
	m_pib->argument = m_pib->argument_data;
	memcpy(m_pib->argument_data, m_argument.c_str(), m_path.length());
	memcpy(m_pib->path_data, m_path.c_str(), m_path.length());

	if (Thread::current) {
		Memory::switch_page_directory(Thread::current->parent_process().page_directory());
	}
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
	unsigned id = pid_bitmap->find_first_clear();
	pid_bitmap->set(id);
	return id;
}

void Process::initiate_process(uintptr_t __pcb)
{
	Process* pcb = reinterpret_cast<Process*>(__pcb);

	auto&& executable_entrypoint = pcb->load_executable(pcb->m_path);
	if (executable_entrypoint.is_error()) {
		warn() << "couldn't load the process, error: " << executable_entrypoint.error();
		pcb->terminate(executable_entrypoint.error());
		return;
	}

	if (pcb->m_privilege_level == ProcessPrivilege::User) {
		void* thread_user_stack = Memory::alloc(STACK_SIZE, MEMORY_TYPE::WRITABLE);

		Context::enter_usermode(executable_entrypoint.value(), uintptr_t(thread_user_stack) + STACK_SIZE - 4);
	} else if (pcb->m_privilege_level == ProcessPrivilege::Kernel) {
		ASSERT_NOT_REACHABLE(); // TODO: kernel process.
	}

	ASSERT_NOT_REACHABLE();
}

void Process::terminate(int status_code)
{
	for (auto&& thread : m_threads) {
		thread->terminate();
	}

	ScopedLock local_lock(m_lock);

	m_state = ProcessState::Terminated;
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

size_t Process::pid()
{
	ScopedLock local_lock(m_lock);
	return m_pid;
}

String Process::name()
{
	ScopedLock local_lock(m_lock);
	return m_name;
}

String Process::path()
{
	ScopedLock local_lock(m_lock);
	return m_path;
}

ProcessPrivilege Process::privilege_level()
{
	ScopedLock local_lock(m_lock);
	return m_privilege_level;
}

uintptr_t Process::page_directory()
{
	ScopedLock local_lock(m_lock);
	return m_page_directory;
}

Process const* Process::parent()
{
	ScopedLock local_lock(m_lock);
	return m_parent;
}

ProcessState Process::state()
{
	ScopedLock local_lock(m_lock);
	return m_state;
}

HandlesManager& Process::handles()
{
	// FIXME: Handle Manager is not locked yet, it should use process's lock to prevent race condition
	//		 when you try to add handle to the process while the process is being destroyed.
	ScopedLock local_lock(m_lock);
	return m_handles;
}

uintptr_t Process::pib()
{
	ScopedLock local_lock(m_lock);
	return reinterpret_cast<uintptr_t>(m_pib);
}

void Process::list_new_thread(Thread& thread)
{
	ScopedLock local_lock(m_lock);
	m_threads.emplace_back(thread);
}

void Process::unlist_new_thread(Thread& thread)
{
	ScopedLock local_lock(m_lock);

	m_threads.remove_if([&](Reference<Thread>& cur) {
		if (cur->tid() == thread.tid())
			return true;
		else
			return false;
	});
}
