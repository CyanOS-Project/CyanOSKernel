#pragma once

#include "Arch/x86/context.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/spinlock.h"
#include "utils/bitmap.h"
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

typedef struct ProcessControlBlock_t {
	unsigned pid;
	const char* name;
	uintptr_t page_directory;
	ProcessState state;
	ProcessControlBlock_t* parent;
} ProcessControlBlock;

typedef struct ThreadControlBlock_t {
	unsigned tid;
	unsigned sleep_ticks;
	uintptr_t task_stack_start;
	uintptr_t task_stack_pointer;
	ThreadState state;
	ProcessControlBlock* parent;
} ThreadControlBlock;

typedef void (*thread_function)(uintptr_t argument);

class Scheduler
{
  private:
	static Bitmap* m_tid_bitmap;
	static Bitmap* m_pid_bitmap;
	static CircularQueue<ThreadControlBlock>* ready_threads;
	static CircularQueue<ThreadControlBlock>* sleeping_threads;
	static CircularQueue<ProcessControlBlock>* processes;
	static SpinLock scheduler_lock;
	static void load_context(ISRContextFrame* current_context, const ThreadControlBlock* thread);
	static void switch_page_directory(const uintptr_t page_directory);
	static void save_context(const ISRContextFrame* current_context, ThreadControlBlock* thread);
	static void wake_up_sleepers();
	static void schedule_handler(ISRContextFrame* frame);
	static void select_next_thread();
	static unsigned reserve_tid();
	static unsigned reserve_pid();
	static void create_tcb(uintptr_t task_stack, uintptr_t task_current_stack, ProcessControlBlock* parent);

  public:
	static void create_new_thread(ProcessControlBlock* process, thread_function address, uintptr_t argument);
	static ProcessControlBlock& create_new_process(const char* name);
	static void schedule(ISRContextFrame* current_context, ScheduleType type);
	static void block_current_thread(ThreadState reason, CircularQueue<ThreadControlBlock>* waiting_list);
	static void unblock_thread(CircularQueue<ThreadControlBlock>* waiting_list);
	static void setup();
	static void sleep(unsigned ms);
	static void yield();
};