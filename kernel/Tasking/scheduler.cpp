#include "scheduler.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/gdt.h"
#include "Arch/x86/isr.h"
#include "Arch/x86/panic.h"
#include "Devices/Console/console.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"
#include "utils/assert.h"

CircularList<ThreadControlBlock>* Scheduler::ready_threads;
CircularList<ThreadControlBlock>* Scheduler::sleeping_threads;

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
	ready_threads = new CircularList<ThreadControlBlock>;
	sleeping_threads = new CircularList<ThreadControlBlock>;
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
	ThreadControlBlock& current_TCB = ready_threads->head_data();
	if (type == ScheduleType::TIMED)
		wake_up_sleepers();
	// TODO: remove blocked threads
	if (current_TCB.state != ThreadState::READY) {
		save_context(current_context);
		current_TCB.state = ThreadState::READY;
	}
	// FIXME: schedule idle if there is no ready thread
	CircularList<ThreadControlBlock>::Iterator iterator = CircularList<ThreadControlBlock>::Iterator(ready_threads);
	select_next_thread(iterator);
	ready_threads->set_head(iterator);
	ready_threads->head_data().state = ThreadState::RUNNING;
	load_context(current_context);
	spinlock_release(&scheduler_lock);
}

// Round Robinson Scheduling Algorithm.
void Scheduler::select_next_thread(CircularList<ThreadControlBlock>::Iterator& iterator)
{
	iterator++;
}

// Decrease sleep_ticks of each thread and wake up whose value is zero.
void Scheduler::wake_up_sleepers()
{
	if (sleeping_threads->is_empty())
		return;
	CircularList<ThreadControlBlock>::Iterator iterator = CircularList<ThreadControlBlock>::Iterator(sleeping_threads);
	do {
		ThreadControlBlock& current = sleeping_threads->data(iterator);
		if (current.sleep_ticks > 0) {
			current.sleep_ticks--;
			if (!current.sleep_ticks) {
				current.state = ThreadState::READY;
				sleeping_threads->remove(iterator);
				ready_threads->push_back(current);
				wake_up_sleepers();
				break;
			}
		}
		iterator++;
	} while (!iterator.is_head());
}

// Put the current thread into sleep for ms.
void Scheduler::sleep(unsigned ms)
{

	spinlock_acquire(&scheduler_lock);
	ThreadControlBlock& current = sleeping_threads->head_data();
	current.sleep_ticks = ms;
	current.state = ThreadState::BLOCKED_SLEEP;
	sleeping_threads->push_back(current);
	spinlock_release(&scheduler_lock);
	yield();
}

void Scheduler::block_current_thread(ThreadState reason)
{
	spinlock_acquire(&scheduler_lock);
	ThreadControlBlock& current = ready_threads->head_data();
	current.state = ThreadState::BLOCKED_LOCK;
	spinlock_release(&scheduler_lock);
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
	ThreadControlBlock new_thread;
	memset((char*)&new_thread, 0, sizeof(ThreadControlBlock));
	void* thread_stack = (void*)Memory::alloc(STACK_SIZE, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	ContextFrame* frame = (ContextFrame*)((unsigned)thread_stack + STACK_SIZE - sizeof(ContextFrame));
	// frame->esp = (unsigned)frame + 4;
	frame->eip = address;
	frame->cs = KCS_SELECTOR;
	frame->eflags = 0x202;
	new_thread.tid = tid++;
	new_thread.context.esp = (unsigned)frame + 4;
	new_thread.state = ThreadState::READY;
	ready_threads->push_back(new_thread);
	spinlock_release(&scheduler_lock);
}

// Switch the returned context of the current IRQ.
void Scheduler::load_context(ContextFrame* current_context)
{
	current_context->esp = ready_threads->head_data().context.esp;
}

// Save current context into its TCB.
void Scheduler::save_context(ContextFrame* current_context)
{
	ready_threads->head_data().context.esp = current_context->esp;
}

// Switch to page directory
void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}
