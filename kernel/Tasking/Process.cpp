#include "Process.h"
#include "Arch/x86/Context.h"
#include "Filesystem/VirtualFilesystem.h"
#include "ScopedLock.h"
#include "Thread.h"
#include "VirtualMemory/Memory.h"
#include <Assert.h>

List<Process> Process::processes;
StaticBitmap<MAX_BITMAP_SIZE> Process::pid_bitmap;
Spinlock Process::global_lock;

Process& Process::create_virtual_process(StringView name)
{
	ScopedLock local_lock(global_lock);
	auto& pcb = processes.emplace_back(name);
	return pcb;
}

Process& Process::create_new_process(PathView path, StringView argument, ProcessPrivilege privilege)
{
	ScopedLock local_lock(global_lock);
	auto& pcb = processes.emplace_back(path[-1], path, argument, privilege);
	return pcb;
}

Result<Process&> Process::get_process_from_pid(size_t pid)
{
	for (auto&& i : processes) {
		if (i.m_pid == pid) {
			return i;
		}
	}
	return ResultError(ERROR_INVALID_PID);
}

Process::Process(StringView name) :
    m_lock{},
    m_singal_waiting_queue{},
    m_pid{reserve_pid()},
    m_name{name},
    m_path{"/"},
    m_argument{""},
    m_privilege_level{ProcessPrivilege::Kernel},
    m_page_directory{Memory::create_new_virtual_space()},
    m_descriptor_references{0},
    m_parent{nullptr},
    m_state{ProcessState::Ready},
    m_handles{},
    m_threads{},
    m_pib{nullptr}
{
}

Process::Process(StringView name, PathView path, StringView argument, ProcessPrivilege privilege) :
    m_lock{},
    m_singal_waiting_queue{},
    m_pid{reserve_pid()},
    m_name{name},
    m_path{path.full_path()},
    m_argument{argument},
    m_privilege_level{privilege},
    m_page_directory{Memory::create_new_virtual_space()},
    m_descriptor_references{0},
    m_parent{nullptr},
    m_state{ProcessState::Ready},
    m_handles{},
    m_threads{},
    m_pib{nullptr}
{
	Memory::switch_page_directory(m_page_directory);

	m_pib = static_cast<UserProcessInformationBlock*>(
	    valloc(0, sizeof(UserProcessInformationBlock), PAGE_USER | PAGE_READWRITE));

	m_pib->pid = m_pid;
	m_pib->path = m_pib->path_data;
	m_pib->argument = m_pib->argument_data;
	memcpy(m_pib->argument_data, m_argument.c_str(), m_argument.length());
	memcpy(m_pib->path_data, m_path.c_str(), m_path.length());
	m_pib->argument_data[m_argument.length()] = 0;
	m_pib->path_data[m_path.length()] = 0;

	if (Thread::current) {
		Memory::switch_page_directory(Thread::current->parent_process().page_directory());
	}

	Thread::create_thread(
	    *this, [this]() { initiate_process(); }, ThreadPrivilege::Kernel);
}

Process::~Process()
{
	ASSERT(pid_bitmap.check_set(m_pid));
	pid_bitmap.clear(m_pid);

	// free_page_direcotry(); FIXME: delete page directory.
}

size_t Process::pid()
{
	ScopedLock local_lock(m_lock);
	return m_pid;
}

const String& Process::name()
{
	ScopedLock local_lock(m_lock);
	return m_name;
}

const String& Process::path()
{
	ScopedLock local_lock(m_lock);
	return m_path;
}

ProcessPrivilege Process::privilege_level()
{
	ScopedLock local_lock(m_lock);
	return m_privilege_level;
}

uptr Process::page_directory()
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

uptr Process::pib()
{
	ScopedLock local_lock(m_lock);
	return reinterpret_cast<uptr>(m_pib);
}

void Process::terminate(int status_code)
{
	// FIXME: m_threads are not protected from race conditions.
	for (auto&& thread : m_threads) {
		thread->terminate();
	}

	ScopedLock local_lock(m_lock);

	ASSERT(m_state != ProcessState::Zombie);

	m_state = ProcessState::Zombie;
	m_return_status = status_code;
	m_singal_waiting_queue.wake_up_all();

	if (m_descriptor_references == 0) {
		cleanup();
	}
}

int Process::wait_for_signal()
{
	ScopedLock local_lock(m_lock);
	if (m_state != ProcessState::Zombie) {
		m_singal_waiting_queue.wait(local_lock);
	}
	return m_return_status;
}

void Process::list_new_thread(Thread& thread)
{
	ScopedLock local_lock(m_lock);
	m_threads.emplace_back(thread);
}

void Process::unlist_new_thread(Thread& thread)
{
	ScopedLock local_lock(m_lock);

	m_threads.remove_if([&](Reference<Thread>& cur) { return cur->tid() == thread.tid(); });
}

void Process::descriptor_referenced()
{
	ScopedLock local_lock(m_lock);
	m_descriptor_references++;
}

void Process::descriptor_dereferenced()
{
	ScopedLock local_lock(m_lock);
	ASSERT(m_descriptor_references);
	m_descriptor_references--;
	if (m_state == ProcessState::Zombie && m_descriptor_references == 0) {
		cleanup();
	}
}

Result<ExecutableInformation> Process::load_executable(PathView path)
{
	ScopedLock local_lock(m_lock);

	auto fd = FileDescription::open(path, OpenMode::OM_READ, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		return ResultError(fd.error());
	}
	FileInfo file_info;
	fd.value()->file_query(file_info);
	// FIXME: use a Buffer data structure here.
	auto buff = UniquePointer(new char[file_info.size]());
	auto result = fd.value()->read(buff.ptr(), file_info.size);
	if (result.is_error()) {
		return ResultError(result.error());
	}

	auto execable_info = ELFLoader::load(buff.ptr(), file_info.size);
	if (execable_info.is_error()) {
		return ResultError(execable_info.error());
	}
	return execable_info.value();
}

void Process::initiate_process()
{
	auto&& execable_info = load_executable(m_path);
	if (execable_info.is_error()) {
		warn() << "couldn't load the process: \"" << m_path << "\" error: " << execable_info.error();
		terminate(execable_info.error());
		return;
	}

	m_pib->constructors_array = execable_info.value().constructors_array;
	m_pib->constructors_array_count = execable_info.value().constructors_array_count;

	if (m_privilege_level == ProcessPrivilege::User) {
		void* thread_user_stack = valloc(0, STACK_SIZE, PAGE_USER | PAGE_READWRITE);

		Context::enter_usermode(execable_info.value().entry_point, uptr(thread_user_stack) + STACK_SIZE - 4);
	} else if (m_privilege_level == ProcessPrivilege::Kernel) {
		ASSERT_NOT_REACHABLE(); // TODO: kernel process.
	}

	ASSERT_NOT_REACHABLE();
}

size_t Process::reserve_pid()
{
	size_t id = pid_bitmap.find_first_clear();
	pid_bitmap.set(id);
	return id;
}

void Process::cleanup()
{
	warn() << "Process " << m_pid << " is freed from memory.";
	processes.remove(processes.find_if([this](auto& process) { return &process == this; }));
}