#include "scheduler.h"

SpinLock Scheduler::scheduler_lock;

void Scheduler::setup()
{
	spinlock_init(&scheduler_lock);
	ISR::register_isr_handler(schedule_handler, SCHEDULE_IRQ);
	SystemCall::setup();
	Process::setup();
	Thread::setup();
	// auto& new_proc = create_new_process("idle_process");
	// create_new_thread(&new_proc, idle, 0);
}

// Select next process, save and switch context.
void Scheduler::schedule(ISRContextFrame* current_context, ScheduleType type)
{
	// FIXME: schedule idle if there is no ready thread
	// TODO: move all unnecessary stuff to a separate thread to be performed later.
	spinlock_acquire(&scheduler_lock);
	if (type == ScheduleType::TIMED)
		wake_up_sleepers();

	if (Thread::current) {
		save_context(current_context, Thread::current);
		Thread::current->m_state = ThreadState::READY;
	}
	select_next_thread();
	Thread& next_thread = Thread::ready_threads->head();
	next_thread.m_state = ThreadState::RUNNING;
	if (Thread::current) {
		if (next_thread.m_parent->m_pid != Thread::current->m_parent->m_pid) {
			switch_page_directory(next_thread.m_parent->m_page_directory);
		}
	} else {
		switch_page_directory(next_thread.m_parent->m_page_directory);
	}
	Thread::current = &next_thread;
	load_context(current_context, &next_thread);
	spinlock_release(&scheduler_lock);
}

// Round Robinson Scheduling Algorithm.
void Scheduler::select_next_thread()
{
	Thread::ready_threads->increment_head();
}

// Decrease sleep_ticks of each thread and wake up whose value is zero.
void Scheduler::wake_up_sleepers()
{
	for (auto thread = Thread::sleeping_threads->begin(); thread != Thread::sleeping_threads->end(); ++thread) {
		if (thread->m_sleep_ticks <= PIT::ticks) {
			thread->m_sleep_ticks = 0;
			Thread::sleeping_threads->move_to_other_list(Thread::ready_threads, thread);
			wake_up_sleepers();
			break;
		}
	}
}

// Switch the returned context of the current IRQ.
void Scheduler::load_context(ISRContextFrame* current_context, const Thread* thread)
{
	current_context->context_stack = thread->m_kernel_stack_pointer;
	Context::switch_task_stack(thread->m_kernel_stack_end);
}

// Save current context into its TCB.
void Scheduler::save_context(const ISRContextFrame* current_context, Thread* thread)
{
	thread->m_kernel_stack_pointer = current_context->context_stack;
}

// Switch to page directory
void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}

void Scheduler::schedule_handler(ISRContextFrame* frame)
{
	schedule(frame, ScheduleType::FORCED);
}
