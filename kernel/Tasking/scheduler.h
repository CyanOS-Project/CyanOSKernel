#pragma once

#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/spinlock.h"
#include "utils/list.h"
#include "utils/types.h"

#define STACK_SIZE PAGE_SIZE

enum class ThreadState {
	RUNNING,
	READY,
	BLOCKED_SLEEP,
	BLOCKED_LOCK,
	BLOCKED_IO,
	SUSPENDED,
};
enum class ProcessState {
	RUNNING,
	ACTIVE,
	BLOCKED,
	SUSPENDED,
};

enum class ScheduleType {
	FORCED,
	TIMED,
};

typedef struct RegistersContext_t {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eip;
	uint32_t eflags;
} RegistersContext;

typedef struct ProcessControlBlock_t {
	unsigned pid;
	unsigned page_directory;
	ProcessState state;
	ProcessControlBlock_t* parent;
} ProcessControlBlock;

typedef struct ThreadControlBlock_t {
	unsigned tid;
	unsigned sleep_ticks;
	ThreadState state;
	RegistersContext context;
	ProcessControlBlock_t* parent;
} ThreadControlBlock;

class Scheduler
{
  private:
	static CircularList<ThreadControlBlock>* ready_threads;
	static CircularList<ThreadControlBlock>* sleeping_threads;
	static SpinLock scheduler_lock;
	static void load_context(ContextFrame* current_context, const ThreadControlBlock* thread);
	static void switch_page_directory(const uintptr_t page_directory);
	static void save_context(const ContextFrame* current_context, ThreadControlBlock* thread);
	static void wake_up_sleepers();
	static void schedule_handler(ContextFrame* frame);
	static void select_next_thread(CircularList<ThreadControlBlock>::Iterator& iterator);

  public:
	static void create_new_thread(void* address);
	static void schedule(ContextFrame* current_context, ScheduleType type);
	static void block_current_thread(ThreadState reason, CircularList<ThreadControlBlock>* waiting_list);
	static void unblock_thread(CircularList<ThreadControlBlock>* waiting_list);
	static void setup();
	static void sleep(unsigned ms);
	static void yield();
};