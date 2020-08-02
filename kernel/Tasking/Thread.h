#pragma once
#include "Arch/x86/context.h"
#include "Arch/x86/spinlock.h"
#include "Devices/Timer/pit.h"
#include "Process.h"
#include "VirtualMemory/memory.h"
#include "utils/IterationDecision.h"
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
	static void idle(_UNUSED_PARAM(uintptr_t));
	static Bitmap* m_tid_bitmap;
	static CircularQueue<Thread>* ready_threads;
	static CircularQueue<Thread>* sleeping_threads;

	SpinLock m_lock;
	ThreadState m_state;
	unsigned reserve_tid();

  public:
	static Thread* current;
	static void create_thread(const Process& parent_process, thread_function address, uintptr_t argument);

	unsigned tid;
	unsigned sleep_ticks;
	uintptr_t kernel_stack_start;
	uintptr_t kernel_stack_end;
	uintptr_t kernel_stack_pointer;
	uintptr_t user_stack_start;
	const Process& parent;

	Thread(const Process& process, thread_function address, uintptr_t argument);
	~Thread();
	void block(ThreadState reason);
	void suspend(int reason);
	void resume();
	void terminate();
	void get_process();
	void get_tid();
	static void sleep(unsigned ms);
	static void yield();
	static void setup();

	inline static number_of_read_threads()
	{
		return ready_threads->size();
	}

	template <typename Callback> static void for_each_sleeping(Callback callback)
	{
		for (auto&& thread = Thread::sleeping_threads->begin(); thread != Thread::sleeping_threads->end(); ++thread) {
			auto ret = callback(*thread);
			if (ret == IterationDecision::Break) {
				break;
			} else if (ret == IterationDecision::Restart) {
				for_each_sleeping(callback);
				break;
			}
		}
	}

	template <typename Callback> static void for_each_ready(Callback callback)
	{
		for (auto&& thread = Thread::ready_threads->begin(); thread != Thread::ready_threads->end(); ++thread) {
			auto ret = callback(*thread);
			if (ret == IterationDecision::Break) {
				break;
			} else if (ret == IterationDecision::Restart) {
				for_each_sleeping(callback);
				break;
			}
		}
	}
};
