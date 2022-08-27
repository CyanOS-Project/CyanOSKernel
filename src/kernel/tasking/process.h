#pragma once

#include "arch/x86/spinlock.h"
#include "filesystem/handles_manager.h"
#include "loader/elf.h"
#include "process_definitions.h"
#include "wait_queue.h"
#include <bitmap.h>
#include <list.h>
#include <path_view.h>
#include <reference.h>
#include <result.h>
#include <string.h>
#include <string_view.h>
#include <types.h>
#include <unique_pointer.h>

enum class ProcessState
{
	Ready,
	Blocked,
	Suspended,
	Zombie,
};
enum class ProcessPrivilege
{
	Kernel,
	User
};

class Thread;
class Process
{
  public:
	static Process& create_virtual_process(StringView name);
	static Process& create_new_process(PathView path, StringView argument, ProcessPrivilege privilege);
	static Result<Process&> get_process_from_pid(size_t pid);

	size_t pid();
	const String& name();
	const String& path();
	ProcessPrivilege privilege_level();
	uintptr_t page_directory();
	Process const* parent();
	ProcessState state();
	HandlesManager& handles();
	uintptr_t pib();
	void terminate(int status_code);
	int wait_for_signal();
	void list_new_thread(Thread& thread);
	void unlist_new_thread(Thread& thread);
	void descriptor_referenced();
	void descriptor_dereferenced();
	~Process();

  private:
	static Bitmap pid_bitmap;
	static List<Process> processes;
	static Spinlock global_lock;

	Spinlock m_lock;
	WaitQueue m_singal_waiting_queue;
	const size_t m_pid;
	const String m_name;
	const String m_path;
	const String m_argument;
	const ProcessPrivilege m_privilege_level;
	const uintptr_t m_page_directory;
	size_t m_descriptor_references;
	Process const* m_parent;
	ProcessState m_state;
	HandlesManager m_handles;
	List<Reference<Thread>> m_threads; // FIXME: convert it to a vector.
	UserProcessInformationBlock* m_pib;
	int m_return_status;

	Process(StringView name, PathView path, StringView argument, ProcessPrivilege privilege);
	Process(StringView name);
	void initiate_process();
	Result<ExecutableInformation> load_executable(PathView path);
	size_t reserve_pid();
	void cleanup();
	friend class List<Process>;
};
