#include "scheduler.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/panic.h"
#include "Devices/Console/console.h"
#include "Devices/Timer/pit.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "utils/assert.h"

CircularQueue<ThreadControlBlock>* Scheduler::ready_threads;
CircularQueue<ThreadControlBlock>* Scheduler::sleeping_threads;
CircularQueue<ProcessControlBlock>* Scheduler::processes;
ThreadControlBlock* current_thread;
SpinLock Scheduler::scheduler_lock;
Bitmap* Scheduler::m_tid_bitmap;
Bitmap* Scheduler::m_pid_bitmap;

void idle(_UNUSED_PARAM(uintptr_t))
{
	while (1) {
		HLT();
	}
}

void Scheduler::setup()
{
	spinlock_init(&scheduler_lock);
	ready_threads = new CircularQueue<ThreadControlBlock>;
	sleeping_threads = new CircularQueue<ThreadControlBlock>;
	processes = new CircularQueue<ProcessControlBlock>;
	m_tid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	m_pid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	current_thread = nullptr;
	ISR::register_isr_handler(schedule_handler, SCHEDULE_IRQ);
	auto& new_proc = create_new_process();
	create_new_thread(&new_proc, idle, 0);
}

void Scheduler::schedule_handler(ContextFrame* frame)
{
	schedule(frame, ScheduleType::FORCED);
}

// Select next process, save and switch context.
void Scheduler::schedule(ContextFrame* current_context, ScheduleType type)
{
	spinlock_acquire(&scheduler_lock);
	if (type == ScheduleType::TIMED)
		wake_up_sleepers();

	if (current_thread) {
		save_context(current_context, current_thread);
		current_thread->state = ThreadState::READY;
	}
	// FIXME: schedule idle if there is no ready thread
	select_next_thread();
	ThreadControlBlock& next_thread = ready_threads->head();
	next_thread.state = ThreadState::RUNNING;
	if (current_thread) {
		if (next_thread.parent->pid != current_thread->parent->pid) {
			switch_page_directory(next_thread.parent->page_directory);
		}
	}
	current_thread = &next_thread;
	load_context(current_context, &next_thread);
	spinlock_release(&scheduler_lock);
}

// Round Robinson Scheduling Algorithm.
void Scheduler::select_next_thread()
{
	ready_threads->increment_head();
}

// Decrease sleep_ticks of each thread and wake up whose value is zero.
void Scheduler::wake_up_sleepers()
{
	for (CircularQueue<ThreadControlBlock>::Iterator thread = sleeping_threads->begin();
	     thread != sleeping_threads->end(); ++thread) {
		if (thread->sleep_ticks <= PIT::ticks) {
			thread->sleep_ticks = 0;
			sleeping_threads->move_to_other_list(ready_threads, thread);
			wake_up_sleepers();
			break;
		}
	}
}

// Put the current thread into sleep for ms.
void Scheduler::sleep(unsigned ms)
{

	spinlock_acquire(&scheduler_lock);
	ThreadControlBlock& current = ready_threads->head();
	current.sleep_ticks = PIT::ticks + ms;
	current.state = ThreadState::BLOCKED_SLEEP;
	ready_threads->move_head_to_other_list(sleeping_threads);
	spinlock_release(&scheduler_lock);
	yield();
}

void Scheduler::block_current_thread(ThreadState reason, CircularQueue<ThreadControlBlock>* waiting_list)
{
	spinlock_acquire(&scheduler_lock);
	ThreadControlBlock& current = ready_threads->head();
	current.state = ThreadState::BLOCKED_LOCK;
	ready_threads->move_head_to_other_list(waiting_list);
	spinlock_release(&scheduler_lock);
}

void Scheduler::unblock_thread(CircularQueue<ThreadControlBlock>* waiting_list)
{
	spinlock_acquire(&scheduler_lock);
	waiting_list->move_head_to_other_list(ready_threads);
	spinlock_release(&scheduler_lock);
}

// schedule another thread.
void Scheduler::yield()
{
	asm volatile("int $0x81");
}

// Create thread structure of a new thread
void Scheduler::create_new_thread(ProcessControlBlock* process, thread_function address, uintptr_t argument)
{
	spinlock_acquire(&scheduler_lock);
	ThreadControlBlock new_thread;
	memset((char*)&new_thread, 0, sizeof(ThreadControlBlock));
	void* thread_stack = Memory::alloc(STACK_SIZE, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	InitialThreadStack* init_thread_stack =
	    (InitialThreadStack*)((unsigned)thread_stack + STACK_SIZE - sizeof(InitialThreadStack));
	init_thread_stack->return_address = idle;
	init_thread_stack->argument = argument;
	init_thread_stack->frame.eip = (uintptr_t)address;
	init_thread_stack->frame.cs = KCS_SELECTOR;
	init_thread_stack->frame.eflags = 0x202;
	new_thread.tid = Scheduler::reserve_tid();
	new_thread.task_stack = (intptr_t)thread_stack;
	new_thread.context.esp = (unsigned)&init_thread_stack->frame + 4;
	new_thread.state = ThreadState::READY;
	if (process) {
		new_thread.parent = process;
	} else {
		new_thread.parent = current_thread->parent;
	}
	ready_threads->push_back(new_thread);
	spinlock_release(&scheduler_lock);
}

ProcessControlBlock& Scheduler::create_new_process()
{
	spinlock_acquire(&scheduler_lock);
	ProcessControlBlock new_process;
	memset(&new_process, 0, sizeof(ProcessControlBlock));
	new_process.parent = 0;
	new_process.pid = reserve_pid();
	new_process.page_directory = Memory::create_new_virtual_space();
	processes->push_front(new_process);
	auto& pcb = processes->head();
	spinlock_release(&scheduler_lock);
	return pcb;
}

unsigned Scheduler::reserve_tid()
{
	unsigned id = m_tid_bitmap->find_first_unused();
	m_tid_bitmap->set_used(id);
	return id;
}

unsigned Scheduler::reserve_pid()
{
	unsigned id = m_pid_bitmap->find_first_unused();
	m_pid_bitmap->set_used(id);
	return id;
}

// Switch the returned context of the current IRQ.
void Scheduler::load_context(ContextFrame* current_context, const ThreadControlBlock* thread)
{
	current_context->esp = thread->context.esp;
	GDT::set_tss_stack(thread->task_stack);
}

// Save current context into its TCB.
void Scheduler::save_context(const ContextFrame* current_context, ThreadControlBlock* thread)
{
	thread->context.esp = current_context->esp;
}

// Switch to page directory
void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}
