#include "Thread.h"
#include "Arch/x86/Context.h"
#include "Devices/Timer/Pit.h"
#include "Process.h"
#include "ScopedLock.h"
#include "VirtualMemory/Memory.h"
#include "WaitQueue.h"
#include <Assert.h>

Thread* Thread::current = nullptr;
IntrusiveList<Thread> Thread::ready_threads;
IntrusiveList<Thread> Thread::sleeping_threads;
IntrusiveList<Thread> Thread::blocked_threads;
IntrusiveList<Thread> Thread::blocked_timed_threads;
Bitmap<MAX_BITMAP_SIZE> Thread::tid_bitmap;
Spinlock Thread::global_lock;

Thread& Thread::create_thread(Process& process, Function<void()> entry_point, ThreadPrivilege priv)
{
	ScopedLock local_lock(global_lock);
	Thread& new_thread = ready_threads.push_back(*new Thread(process, move(entry_point), priv));
	process.list_new_thread(new_thread);

	return new_thread;
}

Thread& Thread::create_init_thread(Process& process)
{
	ASSERT(current == nullptr);

	auto fake_callable_object = []() {};
	auto& new_thread = create_thread(process, fake_callable_object, ThreadPrivilege::Kernel);
	current = &new_thread;
	return new_thread;
}

void Thread::wake_up_sleepers()
{
	auto&& thread1 = Thread::sleeping_threads.begin();
	while (thread1 != Thread::sleeping_threads.end()) {
		auto iterator_copy = thread1++;

		if (iterator_copy->m_sleep_ticks <= PIT::ticks) {
			iterator_copy->m_sleep_ticks = 0;
			iterator_copy->wake_up();
		}
	}

	auto&& thread2 = Thread::blocked_timed_threads.begin();
	while (thread2 != Thread::blocked_timed_threads.end()) {
		auto iterator_copy = thread2++;

		if (iterator_copy->m_sleep_ticks <= PIT::ticks) {
			iterator_copy->m_sleep_ticks = 0;
			iterator_copy->m_blocker->handle_thread_timeout(*iterator_copy);
			iterator_copy->wake_up();
		}
	}
}

Thread::Thread(Process& process, Function<void()> entry_point, ThreadPrivilege priv) :
    m_tid{reserve_tid()},
    m_parent{process},
    m_state{ThreadState::Ready},
    m_privilege{priv},
    m_entry_point{move(entry_point)}
{
	void* thread_kernel_stack = valloc(0, STACK_SIZE, PAGE_READWRITE);

	uintptr_t stack_pointer = 0;
	ContextInformation info = {.stack = thread_kernel_stack,
	                           .stack_size = STACK_SIZE,
	                           .start_function = uptr(thread_start),
	                           .return_function = uptr(nullptr),
	                           .argument = uptr(this)};

	if (priv == ThreadPrivilege::Kernel) {
		stack_pointer = Context::setup_task_stack_context(ContextType::Kernel, info);
	} else {
		stack_pointer = Context::setup_task_stack_context(ContextType::User, info);
	}

	m_kernel_stack_start = uintptr_t(thread_kernel_stack);
	m_kernel_stack_end = m_kernel_stack_start + STACK_SIZE;
	m_kernel_stack_pointer = stack_pointer;
	m_state = ThreadState::Ready;

	if (current && (m_parent.pid() != Thread::current->m_parent.pid())) {
		Memory::switch_page_directory(m_parent.page_directory());

		m_tib = reinterpret_cast<UserThreadInformationBlock*>(
		    valloc(0, sizeof(UserThreadInformationBlock), PAGE_USER | PAGE_READWRITE));
		m_tib->tid = m_tid;

		Memory::switch_page_directory(Thread::current->m_parent.page_directory());
	} else {
		m_tib = reinterpret_cast<UserThreadInformationBlock*>(
		    valloc(0, sizeof(UserThreadInformationBlock), PAGE_USER | PAGE_READWRITE));
		m_tib->tid = m_tid;
	}
}

Thread::~Thread()
{
	ASSERT(tid_bitmap.check_set(m_tid));
	tid_bitmap.clear(m_tid);
}

void Thread::thread_start(Thread* thread)
{
	thread->m_entry_point();
	while (1) {
		HLT();
	}
	// FIXME: there is bug in thread termination.
	// thread->terminate();
}

void Thread::wake_up()
{
	ScopedLock local_lock(global_lock);

	switch (m_state) {
		case ThreadState::BlockedSleep: {
			sleeping_threads.remove(*this);
			break;
		}

		case ThreadState::BlockedQueue: {
			blocked_threads.remove(*this);
			break;
		}

		case ThreadState::BlockedQueueTimed: {
			blocked_timed_threads.remove(*this);
			break;
		}
		default:
			break;
	}

	ready_threads.push_back(*this);
	m_state = ThreadState::Ready;
	m_blocker = nullptr;
}

void Thread::block(WaitQueue& blocker, size_t ms_timeout)
{
	ScopedLock local_lock(global_lock);

	ready_threads.remove(*this);

	if (ms_timeout) {
		m_sleep_ticks = PIT::ticks + ms_timeout;
		m_state = ThreadState::BlockedQueueTimed;
		blocked_timed_threads.push_back(*this);
	} else {
		m_state = ThreadState::BlockedQueue;
		blocked_threads.push_back(*this);
	}

	m_blocker = &blocker;
}

void Thread::sleep(size_t ms)
{
	ScopedLock local_lock(global_lock);

	ready_threads.remove(*this);

	m_sleep_ticks = PIT::ticks + ms;
	m_state = ThreadState::BlockedSleep;
	sleeping_threads.push_back(*this);

	local_lock.release();

	yield();
}

void Thread::yield()
{
	asm volatile("int $0x81");
}

size_t Thread::reserve_tid()
{
	size_t id = tid_bitmap.find_first_clear();
	tid_bitmap.set(id);
	return id;
}

void Thread::terminate()
{
	switch (m_state) {
		case ThreadState::Ready: {
			ScopedLock local_lock(global_lock);

			ready_threads.remove(*this);
			break;
		}
		case ThreadState::BlockedSleep: {
			ScopedLock local_lock(global_lock);

			sleeping_threads.remove(*this);
			break;
		}
		case ThreadState::BlockedQueue: {
			ASSERT(m_blocker);
			ScopedLock local_lock(global_lock);

			blocked_threads.remove(*this);
			m_blocker->handle_thread_terminated(*this);
			break;
		}

		case ThreadState::BlockedQueueTimed: {
			ASSERT(m_blocker);
			ScopedLock local_lock(global_lock);

			blocked_timed_threads.remove(*this);
			m_blocker->handle_thread_terminated(*this);
			break;
		}
		case ThreadState::Suspended: {
			ASSERT_NOT_REACHABLE();
			break;
		}
	}
	m_parent.unlist_new_thread(*this);
	cleanup();
}

size_t Thread::tid() const
{
	return m_tid;
}

Process& Thread::parent_process() const
{
	return m_parent;
}

ThreadState Thread::state() const
{
	ScopedLock local_lock(global_lock);

	return m_state;
}

size_t Thread::number_of_ready_threads()
{
	ScopedLock local_lock(global_lock);

	return ready_threads.size();
}

void Thread::cleanup()
{
	delete this;
}