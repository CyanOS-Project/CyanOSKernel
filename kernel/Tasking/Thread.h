#pragma once
#include "Bitmap.h"
#include "IntrusiveList.h"
#include "IterationDecision.h"
#include "Result.h"
#include "Types.h"
#include "Arch/x86/Spinlock.h"
#include "Process.h"

enum class ThreadState {
	RUNNABLE,
	BLOCKED_SLEEP,
	BLOCKED_QUEUE,
	BLOCKED_IO,
	SUSPENDED,
};
const size_t STACK_SIZE = 0x1000;

class WaitQueue;

class Thread : public IntrusiveListNode<Thread>
{
  private:
	typedef void (*thread_function)(uintptr_t argument);
	static void idle(_UNUSED_PARAM(uintptr_t));
	static Bitmap* m_tid_bitmap;
	static IntrusiveList<Thread>* ready_threads;
	static IntrusiveList<Thread>* sleeping_threads;
	static StaticSpinlock global_lock;

	Spinlock m_lock;
	const unsigned m_tid;
	unsigned m_sleep_ticks;
	uintptr_t m_kernel_stack_start;
	uintptr_t m_kernel_stack_end;
	uintptr_t m_kernel_stack_pointer;
	uintptr_t m_user_stack_start;
	Process& m_parent;
	ThreadState m_state;
	unsigned reserve_tid();
	Thread(Process& process, thread_function address, uintptr_t argument);

  public:
	static Thread* current;
	static Thread& create_thread(Process& parent_process, thread_function address, uintptr_t argument);
	static void sleep(unsigned ms);
	static void yield();
	static void setup();

	void wake_up_from_queue();
	void wake_up_from_sleep();
	void block();
	void terminate();
	~Thread();
	unsigned tid();
	ThreadState state();
	Process& parent_process();

	static size_t number_of_ready_threads();

	template <typename Callback> static void for_each_sleeping(Callback callback)
	{
		auto&& thread = Thread::sleeping_threads->begin();
		while (thread != Thread::sleeping_threads->end()) {
			auto iterator_copy = thread++;
			auto ret = callback(*iterator_copy);
			if (ret == IterationDecision::Break) {
				break;
			}
		}
	}

	template <typename Callback> static void for_each_ready(Callback callback)
	{
		auto&& thread = Thread::ready_threads->begin();
		while (thread != Thread::ready_threads->end()) {
			auto iterator_copy = thread++;
			auto ret = callback(*iterator_copy);
			if (ret == IterationDecision::Break) {
				break;
			}
		}
	}
	friend class Scheduler;
	friend class WaitQueue;
};
