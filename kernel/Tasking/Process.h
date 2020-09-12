#pragma once

#include "Arch/x86/Spinlock.h"
#include "Bitmap.h"
#include "Filesystem/HandlesManager.h"
#include "List.h"
#include "Result.h"
#include "String.h"
#include "StringView.h"
#include "Types.h"
#include "UniquePointer.h"

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

	Process(const StringView& name, const StringView& path);
	unsigned reserve_pid();
	Result<uintptr_t> load_executable(const StringView& path);

  public:
	static Process& create_new_process(const StringView& name, const StringView& path);
	static void setup();

	const unsigned m_pid;
	const String m_name;
	const String m_path;
	const uintptr_t m_page_directory;
	const ProcessState m_state;
	const Process* m_parent;
	HandlesManager handles;
	~Process();

	friend class List<Process>;
};
