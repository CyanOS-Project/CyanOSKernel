#pragma once
#include "Arch/x86/context.h"
#include "Arch/x86/spinlock.h"
#include "Devices/Timer/pit.h"
#include "Process.h"
#include "VirtualMemory/memory.h"
#include "utils/Result.h"
#include "utils/assert.h"
#include "utils/bitmap.h"
#include "utils/list.h"
#include "utils/types.h"

enum class ThreadState {
	RUNNING,
	READY,
	BLOCKED_SLEEP,
	BLOCKED_LOCK,
	BLOCKED_IO,
	SUSPENDED,
};
const size_t STACK_SIZE = 0x1000;

class Thread
{
  private:
	typedef void (*thread_function)(uintptr_t argument);
	SpinLock lock;
	unsigned reserve_tid();
	static void idle(_UNUSED_PARAM(uintptr_t));
	static Bitmap* m_tid_bitmap;

  public:
	// TCB------
	unsigned m_tid;
	unsigned m_sleep_ticks;
	uintptr_t m_kernel_stack_start;
	uintptr_t m_kernel_stack_end;
	uintptr_t m_kernel_stack_pointer;
	uintptr_t m_user_stack_start;
	ThreadState m_state;
	Process* m_parent;
	//------
	static Thread* current;
	static CircularQueue<Thread>* ready_threads;
	static CircularQueue<Thread>* sleeping_threads;
	static void create_thread(Process* parent_process, thread_function address, uintptr_t argument);
	Thread(Process* process, thread_function address, uintptr_t argument);
	~Thread();
	void block(ThreadState reason);
	void suspend(int reason);
	void resume();
	void terminate();
	void sleep(unsigned ms);
	void get_process();
	void get_tid();
	static void yield();
	static void setup();
};
