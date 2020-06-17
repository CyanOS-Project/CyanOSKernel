#include "scheduler.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/panic.h"
#include "Devices/Console/console.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "utils/assert.h"

ThreadControlBlock* Scheduler::active_threads;
ThreadControlBlock* Scheduler::sleeping_threads;
ThreadControlBlock* Scheduler::current_thread;

SpinLock Scheduler::scheduler_lock;

unsigned tid;

void idle()
{
	while (1) {
		HLT();
	}
}
void Scheduler::setup()
{
	spinlock_init(&scheduler_lock);
	tid = 0;
	active_threads = nullptr;
	sleeping_threads = nullptr;
	current_thread = nullptr;
	ISR::register_isr_handler(schedule_handler, SCHEDULE_IRQ);
	create_new_thread((uintptr_t)idle);
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
		save_context(current_context);
		current_thread->state = ThreadState::READY;
	}

	current_thread = active_threads = select_next_thread();
	current_thread->state = ThreadState::RUNNING;
	load_context(current_context);
	spinlock_release(&scheduler_lock);
}

// Round Robinson Scheduling Algorithm.
ThreadControlBlock* Scheduler::select_next_thread()
{
	ASSERT(active_threads);
	return active_threads->next;
}

// Decrease sleep_ticks of each thread and wake up whose value is zero.
void Scheduler::wake_up_sleepers()
{
	ThreadControlBlock* thread_pointer = sleeping_threads;
	ThreadControlBlock* next_thread = 0;

	if (!sleeping_threads)
		return;
	do {
		if (thread_pointer->sleep_ticks > 0) {
			thread_pointer->sleep_ticks--;
			if (!thread_pointer->sleep_ticks) {
				thread_pointer->state = ThreadState::READY;
				delete_from_thread_list(&sleeping_threads, thread_pointer);
				append_to_thread_list(&active_threads, thread_pointer);
				wake_up_sleepers();
				break;
			}
		}
		thread_pointer = thread_pointer->next;
	} while (thread_pointer != sleeping_threads);
}

// Put the current thread into sleep for ms.
void Scheduler::sleep(unsigned ms)
{

	spinlock_acquire(&scheduler_lock);
	current_thread->sleep_ticks = ms;
	current_thread->state = ThreadState::BLOCKED_SLEEP;
	delete_from_thread_list(&active_threads, current_thread);
	append_to_thread_list(&sleeping_threads, current_thread);
	spinlock_release(&scheduler_lock);
	yield();
}

void Scheduler::block_current_thread(ThreadState reason)
{
	spinlock_acquire(&scheduler_lock);
	current_thread->state = ThreadState::BLOCKED_LOCK;
	delete_from_thread_list(&active_threads, current_thread);
	spinlock_release(&scheduler_lock);
	yield();
}

// schedule another thread.
void Scheduler::yield()
{
	asm volatile("int $0x81");
}

// Create thread structure of a new thread
void Scheduler::create_new_thread(uintptr_t address)
{
	spinlock_acquire(&scheduler_lock);
	ThreadControlBlock* new_thread = (ThreadControlBlock*)Heap::kmalloc(sizeof(ThreadControlBlock), 0);
	void* thread_stack = (void*)Memory::alloc(STACK_SIZE, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	ContextFrame* frame = (ContextFrame*)((unsigned)thread_stack + STACK_SIZE - sizeof(ContextFrame));
	// frame->esp = (unsigned)frame + 4;
	frame->eip = address;
	frame->cs = KCS_SELECTOR;
	frame->eflags = 0x202;
	new_thread->tid = tid++;
	new_thread->context.esp = (unsigned)frame + 4;
	new_thread->state = ThreadState::READY;
	append_to_thread_list(&active_threads, new_thread);
	spinlock_release(&scheduler_lock);
}

// Switch the returned context of the current IRQ.
void Scheduler::load_context(ContextFrame* current_context)
{
	current_context->esp = current_thread->context.esp;
}

// Save current context into its TCB.
void Scheduler::save_context(ContextFrame* current_context)
{
	current_thread->context.esp = current_context->esp;
}

// Switch to page directory
void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}

// Append a thread to thread circular list.
void Scheduler::append_to_thread_list(ThreadControlBlock** list, ThreadControlBlock* new_thread)
{
	ASSERT(new_thread)
	ASSERT(list)
	if (*list) {
		new_thread->next = *list;
		new_thread->prev = (*list)->prev;
		(*list)->prev->next = new_thread;
		(*list)->prev = new_thread;
	} else {
		new_thread->next = new_thread->prev = new_thread;
		*list = new_thread;
	}
}

// Delete thread from a list, doesn't free it !!.
void Scheduler::delete_from_thread_list(ThreadControlBlock** list, ThreadControlBlock* thread)
{
	ASSERT(thread)
	ASSERT(list)
	if (thread->next == thread) {
		*list = 0;
	} else {
		thread->prev->next = thread->next;
		thread->next->prev = thread->prev;
		if (*list == thread)
			*list = (*list)->next;
	}
	ASSERT(active_threads)
}