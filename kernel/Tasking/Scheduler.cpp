#include "Scheduler.h"
#include "Arch/x86/Asm.h"
#include "Arch/x86/Isr.h"
#include "Devices/Timer/Pit.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Loader/PE.h"
#include "Process.h"
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

	ASSERT(Thread::current);

	if (type == ScheduleType::TIMED) {
		wake_up_sleepers();
	}

	save_context(current_context, Thread::current);
	Thread& next_thread = select_next_thread();
	if (&next_thread != Thread::current) { // check if we really need to switch
		if (next_thread.m_parent.pid != Thread::current->m_parent.pid) {
			switch_page_directory(next_thread.m_parent.page_directory);
		}
		Thread::current = &next_thread;
		load_context(current_context, &next_thread);
	}
}

Thread& Scheduler::select_next_thread()
{
	// Simple Round Robinson
	ASSERT(Thread::number_of_ready_threads());
	auto next_thread = ++Thread::ready_threads->current(*Thread::current);
	if (next_thread == Thread::ready_threads->end()) {
		return *Thread::ready_threads->begin();
	} else {
		return *next_thread;
	}
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
