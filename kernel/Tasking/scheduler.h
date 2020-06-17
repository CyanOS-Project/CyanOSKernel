#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "Arch/x86/spinlock.h"
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

typedef volatile struct RegistersContext_t {
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

typedef volatile struct ProcessControlBlock_t {
	unsigned pid;
	unsigned page_directory;
	ProcessState state;
	volatile ProcessControlBlock_t* parent;
	volatile ProcessControlBlock_t *next, *prev;
} ProcessControlBlock;

typedef volatile struct ThreadControlBlock_t {
	unsigned tid;
	unsigned sleep_ticks;
	ThreadState state;
	RegistersContext context;
	volatile ProcessControlBlock_t* parent;
	volatile ThreadControlBlock_t *next, *prev;
} ThreadControlBlock;

class Scheduler
{
  private:
	static void load_context(ContextFrame* current_context);
	static void switch_page_directory(uintptr_t page_directory);
	static void save_context(ContextFrame* current_context);

	static void wake_up_sleepers();
	static void schedule_handler(ContextFrame* frame);
	static ThreadControlBlock* select_next_thread();

  public:
	static ThreadControlBlock* active_threads;
	static ThreadControlBlock* sleeping_threads;
	static ThreadControlBlock* current_thread;
	static SpinLock scheduler_lock;
	static void create_new_thread(uintptr_t address);
	static void schedule(ContextFrame* current_context, ScheduleType type);
	static void block_current_thread(ThreadState reason);
	static void setup();
	static void sleep(unsigned ms);
	static void yield();
	static void delete_from_thread_list(ThreadControlBlock** list, ThreadControlBlock* thread);
	static void append_to_thread_list(ThreadControlBlock** list, ThreadControlBlock* new_thread);
};