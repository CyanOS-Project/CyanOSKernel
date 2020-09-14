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
	RUNNING,
	ACTIVE,
	BLOCKED,
	SUSPENDED,
	TERMINATED,
};

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
	int m_return_status;

	Process(const StringView& name, const StringView& path);
	Result<uintptr_t> load_executable(const StringView& path);
	unsigned reserve_pid();

  public:
	const size_t pid;
	const String name;
	const String path;
	const uintptr_t page_directory;
	const Process* parent;
	ProcessState state;
	HandlesManager handles;
	List<Reference<Thread>> threads; // FIXME: convert it to a vector.

	static Process& create_new_process(const StringView& name, const StringView& path);
	static void setup();
	static Result<Process&> get_process_from_pid(size_t pid);
	void terminate(int status_code);
	int wait_for_signal();

	~Process();

	friend class List<Process>;
};
