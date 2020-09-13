#pragma once

#include "Arch/x86/Spinlock.h"
#include "Filesystem/HandlesManager.h"
#include "WaitQueue.h"
#include <Bitmap.h>
#include <List.h>
#include <Result.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

enum class ProcessState {
	RUNNING,
	ACTIVE,
	BLOCKED,
	SUSPENDED,
};

class Process
{
  private:
	static Bitmap* pid_bitmap;
	static List<Process>* processes;
	static StaticSpinlock global_lock;
	static void initiate_process(uintptr_t pcb);

	Spinlock m_lock;
	WaitQueue m_singal_waiting_queue;
	int m_return_status;

	Process(const StringView& name, const StringView& path);
	Result<uintptr_t> load_executable(const StringView& path);
	unsigned reserve_pid();

  public:
	const size_t m_pid;
	const String m_name;
	const String m_path;
	const uintptr_t m_page_directory;
	const ProcessState m_state;
	const Process* m_parent;
	HandlesManager handles;

	static Process& create_new_process(const StringView& name, const StringView& path);
	static void setup();
	static Result<Process&> get_process_from_pid(size_t pid);
	int wait_for_signal();
	~Process();

	friend class List<Process>;
};
