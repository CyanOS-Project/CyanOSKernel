#pragma once

#include "Arch/x86/Spinlock.h"
#include "Filesystem/HandlesManager.h"
#include "WaitQueue.h"
#include <Bitmap.h>
#include <List.h>
#include <PathView.h>
#include <Reference.h>
#include <Result.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

enum class ProcessState {
	Ready,
	Blocked,
	Suspended,
	Zombie,
};
enum class ProcessPrivilege { Kernel, User };

class Thread;
class Process
{
  public:
	static Process& create_virtual_process(StringView name, ProcessPrivilege privilege);
	static Process& create_new_process(PathView path, StringView argument, ProcessPrivilege privilege);
	static void setup();
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
	struct UserProcessInformationBlock {
		size_t pid;
		char* path;
		char* argument;
		char path_data[MAX_PATH_SIZE];
		char argument_data[MAX_PATH_SIZE];
	};
	static Bitmap<MAX_BITMAP_SIZE>* pid_bitmap;
	static List<Process>* processes;
	static StaticSpinlock global_lock;
	static void initiate_process(uintptr_t pcb);

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
	Result<uintptr_t> load_executable(PathView path);
	size_t reserve_pid();
	void cleanup();
	friend class List<Process>;
};
