#pragma once

#include "Arch/x86/spinlock.h"
#include "utils/List.h"
#include "utils/Result.h"
#include "utils/bitmap.h"
#include "utils/types.h"

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
	static void initiate_process(uintptr_t pcb);

	unsigned reserve_pid();
	Result<uintptr_t> load_executable(const char* path);
	SpinLock lock;

  public:
	//--PCB
	const unsigned m_pid;
	const char* m_name;
	const char* m_path;
	const uintptr_t m_page_directory;
	const ProcessState m_state;
	const Process* m_parent;
	//---------
	static Process& create_new_process(const char* name, const char* path);
	Process(const char* name, const char* path);
	~Process();
	static void setup();
};
