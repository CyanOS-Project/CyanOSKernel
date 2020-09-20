#pragma once
#include "Arch/x86/Spinlock.h"
#include "SpinLock.h"
#include <Bitmap.h>
#include <IntrusiveList.h>
#include <IterationDecision.h>
#include <Result.h>
#include <Types.h>
#include <UniquePointer.h>

enum class ThreadState {
	RUNNABLE,
	BLOCKED_SLEEP,
	BLOCKED_QUEUE,
	SUSPENDED,
};
enum class ThreadPrivilege { Kernel, User };

const size_t STACK_SIZE = 0x1000;

class WaitQueue;
class Process;

class Thread : public IntrusiveListNode<Thread>
{
  private:
	struct UserThreadInformationBlock {
		size_t tid;
	};

	typedef void (*thread_function)(uintptr_t argument);
	static Bitmap* m_tid_bitmap;
	static IntrusiveList<Thread>* ready_threads;
	static IntrusiveList<Thread>* sleeping_threads;
	static StaticSpinlock global_lock;
	static void thread_finishing();

	const size_t m_tid;
	Process& m_parent;
	ThreadState m_state;
	ThreadPrivilege m_privilege;
	WaitQueue* m_blocker;
	UserThreadInformationBlock* m_tib;
	unsigned m_sleep_ticks;
	uintptr_t m_kernel_stack_start;
	uintptr_t m_kernel_stack_end;
	uintptr_t m_kernel_stack_pointer;
	uintptr_t m_user_stack_start;

	unsigned reserve_tid();
	Thread(Process& process, thread_function address, uintptr_t argument, ThreadPrivilege priv);

  public:
	static Thread* current;
	static Thread& create_thread(Process& process, thread_function address, uintptr_t argument, ThreadPrivilege priv);
	static Thread& create_init_thread(Process& process);
	static void sleep(unsigned ms);
	static void yield();
	static void setup();
	static size_t number_of_ready_threads();

	void wake_up_from_queue();
	void wake_up_from_sleep();
	void block(WaitQueue& blocker);
	void terminate();
	unsigned tid();
	ThreadState state();
	Process& parent_process();
	~Thread();

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
	friend class IntrusiveList<Thread>;
};
