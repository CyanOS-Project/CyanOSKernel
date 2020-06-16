#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "types.h"

#define STACK_SIZE PAGE_SIZE
enum class ThreadState {
	RUNNING,
	ACTIVE,
	BLOCKED,
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
	static ThreadControlBlock* active_thread;
	static ThreadControlBlock* blocked_thread;
	static ThreadControlBlock* current_thread;
	static void create_new_thread(uintptr_t address);
	static void load_context(ContextFrame* current_context);
	static void switch_page_directory(uintptr_t page_directory);
	static void save_context(ContextFrame* current_context);
	static void delete_from_thread_list(ThreadControlBlock** list, ThreadControlBlock* thread);
	static void append_to_thread_list(ThreadControlBlock** list, ThreadControlBlock* new_thread);
	static void wake_up_sleepers();
	static void schedule_handler(ContextFrame* frame);
	static ThreadControlBlock* select_next_thread();

  public:
	static void schedule_new_thread(ContextFrame* current_context, ScheduleType type);
	static void setup();
	static void thread_sleep(unsigned ms);
	static void loop();
};