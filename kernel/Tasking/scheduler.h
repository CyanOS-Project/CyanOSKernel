#pragma once

#include "Arch/x86/asm.h"
#include "Arch/x86/context.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/spinlock.h"
#include "Devices/Console/console.h"
#include "Devices/Timer/pit.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Loader/pe.h"
#include "Process.h"
#include "SystemCall.h"
#include "Thread.h"
#include "utils/Result.h"
#include "utils/assert.h"
#include "utils/types.h"

enum class ScheduleType {
	FORCED,
	TIMED,
};

class Scheduler
{
  private:
	static SpinLock scheduler_lock;
	static void load_context(ISRContextFrame* current_context, const Thread* thread);
	static void switch_page_directory(const uintptr_t page_directory);
	static void save_context(const ISRContextFrame* current_context, Thread* thread);
	static void wake_up_sleepers();
	static void schedule_handler(ISRContextFrame* frame);
	static Thread& select_next_thread();

  public:
	static void schedule(ISRContextFrame* current_context, ScheduleType type);
	static void setup();
};