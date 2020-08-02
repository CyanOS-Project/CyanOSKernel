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

void Scheduler::schedule(ISRContextFrame* current_context, ScheduleType type)
{
	// FIXME: schedule idle if there is no ready thread
	// TODO: move all unnecessary stuff to a separate thread to be performed later.
	spinlock_acquire(&scheduler_lock);
	if (type == ScheduleType::TIMED)
		wake_up_sleepers();

	if (Thread::current) {
		save_context(current_context, Thread::current);
		// Thread::current->m_state = ThreadState::READY;
	}
	Thread& next_thread = select_next_thread();
	// next_thread.m_state = ThreadState::RUNNING;
	if (Thread::current) {
		if (next_thread.parent.m_pid != Thread::current->parent.m_pid) {
			switch_page_directory(next_thread.parent.m_page_directory);
		}
	} else {
		switch_page_directory(next_thread.parent.m_page_directory);
	}
	Thread::current = &next_thread;
	load_context(current_context, &next_thread);
	spinlock_release(&scheduler_lock);
}
size_t rr_index = 0;
Thread& Scheduler::select_next_thread()
{
	Thread* next_thread = nullptr;
	size_t threads_index = 0;
	Thread::for_each_ready([&](auto& thread) {
		if (threads_index++ == rr_index % Thread::number_of_read_threads()) {
			next_thread = &thread;
			return IterationDecision::Break;
		}
		return IterationDecision::Continue;
	});
	rr_index = (rr_index + 1) % Thread::number_of_read_threads();
	return *next_thread;
}

void Scheduler::wake_up_sleepers()
{
	Thread::for_each_sleeping([](Thread& thread) {
		if (thread.sleep_ticks <= PIT::ticks) {
			thread.sleep_ticks = 0;
			// Thread::sleeping_threads->move_to_other_list(Thread::ready_threads, &thread);
			thread.resume();
			return IterationDecision::Restart; // FIXME: why restart?
		}
		return IterationDecision::Continue;
	});
}

void Scheduler::load_context(ISRContextFrame* current_context, const Thread* thread)
{
	current_context->context_stack = thread->kernel_stack_pointer;
	Context::switch_task_stack(thread->kernel_stack_end);
}

void Scheduler::save_context(const ISRContextFrame* current_context, Thread* thread)
{
	thread->kernel_stack_pointer = current_context->context_stack;
}

void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}

void Scheduler::schedule_handler(ISRContextFrame* frame)
{
	schedule(frame, ScheduleType::FORCED);
}
