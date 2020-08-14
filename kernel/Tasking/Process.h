#pragma once

#include "Arch/x86/spinlock.h"
#include "Filesystem/FileDescription.h"
#include "utils/List.h"
#include "utils/Result.h"
#include "utils/String.h"
#include "utils/StringView.h"
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
	Result<uintptr_t> load_executable(const StringView& path);
	SpinLock lock;

  public:
	//--PCB
	const unsigned m_pid;
	const String m_name;
	const String m_path;
	const uintptr_t m_page_directory;
	const ProcessState m_state;
	const Process* m_parent;
	List<FileDescription*> file_descriptors;
	//---------
	static Process& create_new_process(const StringView& name, const StringView& path);
	Process(const StringView& name, const StringView& path);
	~Process();
	static void setup();
};
