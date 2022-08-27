#pragma once

#include "arch/x86/context.h"
#include "arch/x86/spinlock.h"
#include "thread.h"
#include <result.h>
#include <types.h>

enum class ScheduleType
{
	Forced,
	Timed,
};

class Scheduler
{
  public:
	static void schedule(ISRContextFrame& current_context, ScheduleType type);
	static void setup();

  private:
	static Spinlock lock;
	static void load_context(ISRContextFrame& current_context, Thread& thread);
	static void switch_page_directory(const uintptr_t page_directory);
	static void save_context(const ISRContextFrame& current_context, Thread& thread);
	static void schedule_handler(ISRContextFrame& frame);
	static Thread& select_next_thread();
};