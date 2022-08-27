#include "scheduler.h"
#include "arch/x86/asm.h"
#include "arch/x86/isr.h"
#include "devices/timer/pit.h"
#include "filesystem/virtual_filesystem.h"
#include "loader/pe.h"
#include "process.h"
#include "scoped_lock.h"
#include "system_call.h"
#include "virtual_memory/memory.h"
#include <assert.h>

Spinlock Scheduler::lock;

void Scheduler::setup()
{
	ISR::register_software_interrupt_handler(schedule_handler, SCHEDULE_IRQ);
	SystemCall::setup();
}

void Scheduler::schedule(ISRContextFrame& current_context, ScheduleType type)
{
	// FIXME: schedule idle if there is no ready thread
	// TODO: move all unnecessary stuff to a separate thread to be performed later.
	ScopedLock local_lock(lock);

	ASSERT(Thread::current);

	if (type == ScheduleType::Timed) {
		Thread::wake_up_sleepers();
	}

	save_context(current_context, *Thread::current);
	Thread& next_thread = select_next_thread();
	if (&next_thread != Thread::current) { // check if we really need to switch
		if (next_thread.m_parent.pid() != Thread::current->m_parent.pid()) {
			switch_page_directory(next_thread.m_parent.page_directory());
		}
		Thread::current = &next_thread;
		load_context(current_context, next_thread);
	}
}

Thread& Scheduler::select_next_thread()
{
	// Simple Round Robinson
	ASSERT(Thread::number_of_ready_threads());
	auto next_thread = ++Thread::ready_threads.find(*Thread::current);
	if (next_thread == Thread::ready_threads.end()) {
		return *Thread::ready_threads.begin();
	} else {
		return *next_thread;
	}
}

void Scheduler::load_context(ISRContextFrame& current_context, Thread& thread)
{
	current_context.context_stack = thread.m_kernel_stack_pointer;
	Context::switch_task_stack(thread.m_kernel_stack_end, uintptr_t(thread.parent_process().pib()),
	                           uintptr_t(thread.m_tib));
}

void Scheduler::save_context(const ISRContextFrame& current_context, Thread& thread)
{
	thread.m_kernel_stack_pointer = current_context.context_stack;
}

void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}

void Scheduler::schedule_handler(ISRContextFrame& frame)
{
	schedule(frame, ScheduleType::Forced);
}
