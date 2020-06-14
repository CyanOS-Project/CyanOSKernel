#include "Arch/x86/isr.h"
#include "Arch/x86/paging.h"
#include "types.h"

#define STACK_SIZE PAGE_SIZE
enum class ThreadState {
	RUNNING,
	ACTIVE,
	BLOCKED,
	SUSPENDED,
	INTIALE,
};
enum class ProcessState {
	RUNNING = 1,
	ACTIVE = 2,
	BLOCKED = 3,
	SUSPENDED = 4,
};

struct ThreadControlBlock;
struct ProcessControlBlock;
struct RegistersContext {
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
};
struct ThreadControlBlock {
	unsigned tid;
	unsigned sleep_ticks;
	ThreadState state;
	ProcessControlBlock* parent;
	RegistersContext context;
	ThreadControlBlock *next, *prev;
};

struct ProcessControlBlock {
	unsigned pid;
	unsigned page_directory;
	ProcessState state;
	ProcessControlBlock* parent;
	ThreadControlBlock* threads;
	ProcessControlBlock *next, *prev;
};

class Scheduler
{
  private:
	static ThreadControlBlock* active_thread;
	static ThreadControlBlock* blocked_thread;
	static ThreadControlBlock* current_thread;
	static void create_new_thread(uintptr_t address);
	static void switch_context(ContextFrame* current_context, ThreadControlBlock* new_thread);
	static void switch_page_directory(uintptr_t page_directory);
	static void save_context(ContextFrame* current_context);
	static void delete_from_thread_list(ThreadControlBlock** list, ThreadControlBlock* thread);
	static void append_to_thread_list(ThreadControlBlock** list, ThreadControlBlock* new_thread);
	static void wake_up_sleepers();
	static ThreadControlBlock* select_next_thread();

  public:
	static void schedule(ContextFrame* current_context);
	static void setup();
	static void thread_sleep(unsigned ms);
};