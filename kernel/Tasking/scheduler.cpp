#include "scheduler.h"
#include "Arch/x86/atomic.h"
#include "Devices/Console/console.h"
#include "Devices/Null/null.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"

ThreadControlBlock* Scheduler::active_thread;
ThreadControlBlock* Scheduler::blocked_thread;
ThreadControlBlock* Scheduler::current_thread;
unsigned tid;

void hello1()
{
	for (size_t i = 0; i < 5; i++) {
		asm("CLI");
		printf("Thread1: (%d).\n", i);
		asm("STI");
	}

	while (1) {
		asm("HLT");
	}
}

void hello2()
{
	int index = 0;
	for (size_t j = 0; j < 10; j++) {
		Scheduler::thread_sleep(1000);
		asm("HLT");
		for (size_t i = 0; i < 3; i++) {
			asm("CLI");
			printf("Thread2: (%d).\n", index);
			index++;
			asm("STI");
		}
	}

	while (1) {
		asm("HLT");
	}
}
void hello3()
{
	for (size_t i = 0; i < 5; i++) {
		asm("CLI");
		printf("Thread3: (%d).\n", i);
		asm("STI");
	}
	while (1) {
		asm("HLT");
	}
}

void Scheduler::setup()
{
	tid = 0;
	active_thread = nullptr;
	blocked_thread = nullptr;
	create_new_thread((uintptr_t)hello1);
	create_new_thread((uintptr_t)hello2);
	create_new_thread((uintptr_t)hello3);
	current_thread = active_thread;
}

// Select next process, save and switch context.
void Scheduler::schedule(ContextFrame* current_context)
{
	wake_up_sleepers();
	if (current_thread->state == ThreadState::RUNNING) {
		save_context(current_context);
		current_thread->state = ThreadState::ACTIVE;
	} else if (current_thread->state == ThreadState::BLOCKED) {
		save_context(current_context);
	}

	volatile ThreadControlBlock* next_thread = select_next_thread();
	next_thread->state = ThreadState::RUNNING;
	switch_context(current_context, (ThreadControlBlock*)next_thread);
	active_thread = (ThreadControlBlock*)next_thread;
	current_thread = active_thread;
}

// Round Robinson Scheduling Algorithm.
ThreadControlBlock* Scheduler::select_next_thread()
{
	return active_thread->next;
}

// Decrease sleep_ticks of each thread and wake up whose value is zero.
void Scheduler::wake_up_sleepers()
{
	ThreadControlBlock* thread_pointer = blocked_thread;
	ThreadControlBlock* next_thread = 0;

	if (!blocked_thread)
		return;
	do {
		if (thread_pointer->sleep_ticks > 0) {
			thread_pointer->sleep_ticks--;
			if (!thread_pointer->sleep_ticks) {
				thread_pointer->state = ThreadState::ACTIVE;
				delete_from_thread_list(&blocked_thread, thread_pointer);
				append_to_thread_list(&active_thread, thread_pointer);
				current_thread = thread_pointer;
				if (!blocked_thread)
					break;
			}
		}
		thread_pointer = thread_pointer->next;
	} while (thread_pointer != blocked_thread);
}

// Put the current thread into sleep for ms.
void Scheduler::thread_sleep(unsigned ms)
{
	DISABLE_INTERRUPTS();
	ThreadControlBlock* thread = active_thread;
	active_thread = active_thread->next;
	thread->sleep_ticks = ms;
	thread->state = ThreadState::BLOCKED;
	delete_from_thread_list(&active_thread, thread);
	append_to_thread_list(&blocked_thread, thread);
	ENABLE_INTERRUPTS();
}

// Create thread structure of a new thread
void Scheduler::create_new_thread(uintptr_t address)
{
	ThreadControlBlock* new_thread = (ThreadControlBlock*)Heap::kmalloc(sizeof(ThreadControlBlock), 0);
	void* thread_stack = (void*)Memory::alloc(STACK_SIZE, MEMORY_TYPE::KERNEL, MEMORY_TYPE::WRITABLE);
	new_thread->tid = tid++;
	new_thread->context.esp = (unsigned)thread_stack + STACK_SIZE;
	new_thread->context.eip = address;
	new_thread->context.eflags = 0x200;
	new_thread->state = ThreadState::INTIALE;
	append_to_thread_list(&active_thread, new_thread);
}

// Switch the returned context of the current IRQ.
void Scheduler::switch_context(ContextFrame* current_context, ThreadControlBlock* new_thread)
{
	current_context->eax = new_thread->context.eax;
	current_context->ebx = new_thread->context.ebx;
	current_context->ecx = new_thread->context.ecx;
	current_context->edx = new_thread->context.edx;
	current_context->esi = new_thread->context.esi;
	current_context->edi = new_thread->context.edi;
	current_context->eip = new_thread->context.eip;
	current_context->ebp = new_thread->context.ebp;
	current_context->useresp = new_thread->context.esp;
	current_context->eflags = new_thread->context.eflags;
}

// Save current context into its TCB.
void Scheduler::save_context(ContextFrame* current_context)
{
	current_thread->context.eax = current_context->eax;
	current_thread->context.ebx = current_context->ebx;
	current_thread->context.ecx = current_context->ecx;
	current_thread->context.edx = current_context->edx;
	current_thread->context.esi = current_context->esi;
	current_thread->context.edi = current_context->edi;
	current_thread->context.ebp = current_context->ebp;
	current_thread->context.eip = current_context->eip;
	current_thread->context.esp = current_context->useresp;
	current_thread->context.eflags = current_context->eflags;
}

// Switch to page directory
void Scheduler::switch_page_directory(uintptr_t page_directory)
{
	Memory::switch_page_directory(page_directory);
}

// Append a thread to thread circular list.
void Scheduler::append_to_thread_list(ThreadControlBlock** list, ThreadControlBlock* new_thread)
{
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

// Delete thread from a list.
void Scheduler::delete_from_thread_list(ThreadControlBlock** list, ThreadControlBlock* thread)
{
	if (thread->prev == thread->next) {
		*list = 0;
	} else {
		thread->prev->next = thread->next;
		thread->next->prev = thread->prev;
	}

	Heap::kfree((uintptr_t)thread);
}