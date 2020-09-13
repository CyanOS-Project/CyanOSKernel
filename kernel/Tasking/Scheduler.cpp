#include "Scheduler.h"
#include "Arch/x86/Asm.h"
#include "Arch/x86/Isr.h"
#include "Devices/Timer/Pit.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Loader/PE.h"
#include "ScopedLock.h"
#include "SystemCall.h"
#include "VirtualMemory/Memory.h"
#include <Assert.h>

StaticSpinlock Scheduler::lock;

void Scheduler::setup()
{
	lock.init();
	ISR::register_isr_handler(schedule_handler, SCHEDULE_IRQ);
	SystemCall::setup();
	Process::setup();
	Thread::setup();
}

void Scheduler::schedule(ISRContextFrame* current_context, ScheduleType type)
{
	// FIXME: schedule idle if there is no ready thread
	// TODO: move all unnecessary stuff to a separate thread to be performed later.
	ScopedLock local_lock(lock);
	if (type == ScheduleType::TIMED)
		wake_up_sleepers();

	if (Thread::current) {
		save_context(current_context, Thread::current);
	}
	Thread& next_thread = select_next_thread();
	if (Thread::current) {
		if (next_thread.m_parent.m_pid != Thread::current->m_parent.m_pid) {
			switch_page_directory(next_thread.m_parent.m_page_directory);
		}
	} else {
		switch_page_directory(next_thread.m_parent.m_page_directory);
	}
	Thread::current = &next_thread;
	load_context(current_context, &next_thread);
}

Thread& Scheduler::select_next_thread()
{
	// HACK: fix this silly way of choosing new process.
	static size_t rr_index = 0;
	Thread* next_thread = nullptr;
	size_t threads_index = 0;
	Thread::for_each_ready([&](auto& thread) {
		if (threads_index++ == rr_index % Thread::number_of_ready_threads()) {
			next_thread = &thread;
			return IterationDecision::Break;
		}
		return IterationDecision::Continue;
	});
	ASSERT(Thread::number_of_ready_threads());
	rr_index = (rr_index + 1) % Thread::number_of_ready_threads();
	return *next_thread;
}

void Scheduler::wake_up_sleepers()
{
	Thread::for_each_sleeping([](Thread& thread) {
		if (thread.m_sleep_ticks <= PIT::ticks) {
			thread.m_sleep_ticks = 0;
			thread.wake_up_from_sleep();
		}
		return IterationDecision::Continue;
	});
}

void Scheduler::load_context(ISRContextFrame* current_context, const Thread* thread)
{
	current_context->context_stack = thread->m_kernel_stack_pointer;
	Context::switch_task_stack(thread->m_kernel_stack_end);
}

void Scheduler::save_context(const ISRContextFrame* current_context, Thread* thread)
{
	thread->m_kernel_stack_pointer = current_context->context_stack;
}

void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}

void Scheduler::schedule_handler(ISRContextFrame* frame)
{
	schedule(frame, ScheduleType::FORCED);
}
