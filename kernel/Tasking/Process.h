#pragma once

#include "Arch/x86/Spinlock.h"
#include "Filesystem/HandlesManager.h"
#include "WaitQueue.h"
#include <Bitmap.h>
#include <List.h>
#include <Reference.h>
#include <Result.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

enum class ProcessState {
	Running,
	Active,
	Blocked,
	Suspended,
	Terminated,
};
enum class ProcessPrivilege { Kernel, User };

class Thread;
class Process
{
  private:
	static Bitmap* pid_bitmap;
	static List<Process>* processes;
	static StaticSpinlock global_lock;
	static void initiate_process(uintptr_t pcb);

	Spinlock m_lock;
	WaitQueue m_singal_waiting_queue;
	const size_t m_pid;
	const String m_name;
	const String m_path;
	const ProcessPrivilege m_privilege_level;
	const uintptr_t m_page_directory;
	Process const* m_parent;
	ProcessState m_state;
	HandlesManager m_handles;
	List<Reference<Thread>> m_threads; // FIXME: convert it to a vector.

	int m_return_status;

	Process(const StringView& name, const StringView& path, ProcessPrivilege privilege);
	Result<uintptr_t> load_executable(const StringView& path);
	unsigned reserve_pid();

  public:
	static Process& create_virtual_process(const StringView& name, ProcessPrivilege privilege);
	static Process& create_new_process(const StringView& name, const StringView& path, ProcessPrivilege privilege);
	static void setup();
	static Result<Process&> get_process_from_pid(size_t pid);

	size_t pid();
	String name();
	String path();
	uintptr_t page_directory();
	Process const* parent();
	ProcessState state();
	HandlesManager& handles();
	void terminate(int status_code);
	int wait_for_signal();
	void list_new_thread(Thread& thread);
	void unlist_new_thread(Thread& thread);
	~Process();

	friend class List<Process>;
};
