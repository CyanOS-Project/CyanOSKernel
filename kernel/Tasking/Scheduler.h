#pragma once

#include "Arch/x86/Context.h"
#include "Arch/x86/Spinlock.h"
#include "Thread.h"
#include <Result.h>
#include <Types.h>

enum class ScheduleType {
	Forced,
	Timed,
};

class Scheduler
{
  public:
	static void schedule(ISRContextFrame& current_context, ScheduleType type);
	static void setup();

  private:
	static StaticSpinlock lock;
	static void load_context(ISRContextFrame& current_context, Thread& thread);
	static void switch_page_directory(const uintptr_t page_directory);
	static void save_context(const ISRContextFrame& current_context, Thread& thread);
	static void wake_up_sleepers();
	static void schedule_handler(ISRContextFrame& frame);
	static Thread& select_next_thread();
};