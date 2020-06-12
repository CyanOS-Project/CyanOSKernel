#include "scheduler.h"
#include "Arch/x86/paging.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"

ThreadControlBlock* Scheduler::current_thread;
unsigned tid = 0;
void Scheduler::setup()
{
	current_thread = 0;
}

void Scheduler::create_new_thread(uintptr_t address)
{
	ThreadControlBlock* new_thread = (ThreadControlBlock*)Heap::kmalloc(sizeof(ThreadControlBlock), 0);
	void* thread_stack = (void*)Memory::alloc(PAGE_SIZE, MEMORY_TYPE::KERNEL, MEMORY_TYPE::WRITABLE);
	append_thread_list(new_thread);
	new_thread->tid = tid++;
	new_thread->context.esp = (unsigned)thread_stack + PAGE_SIZE;
	new_thread->context.eip = address;
	current_thread = new_thread;
}

void Scheduler::append_thread_list(ThreadControlBlock* new_thread)
{
	if (current_thread) {
		new_thread->next = current_thread;
		new_thread->prev = current_thread->prev;
		current_thread->prev = new_thread;
		current_thread->prev->next = new_thread;
	} else {
		new_thread->next = new_thread->prev = new_thread;
	}
}
void Scheduler::schedule(ContextFrame* current_context)
{
	ThreadControlBlock* thread_pointer = current_thread;
	ThreadControlBlock* next_thread = 0;
	do {
		if (thread_pointer->sleep_ticks > 0) {
			thread_pointer->sleep_ticks--;
		}

		if (thread_pointer->state == ThreadState::ACTIVE) {
			/* code */
		}

		thread_pointer = thread_pointer->next;
	} while (thread_pointer != current_thread);
}

/*void Scheduler::thread_tick()
{
    ThreadControlBlock* thread_index = current_thread;
    do {
        if (thread_index->sleep_ticks > 0) {
            thread_index->sleep_ticks--;
        }
        thread_index = thread_index->next;
    } while (thread_index != current_thread);
}*/

void Scheduler::switch_context(ContextFrame* current_context, ThreadControlBlock* new_thread)
{
	current_context->eax = new_thread->context.eax;
	current_context->ebx = new_thread->context.ebx;
	current_context->ecx = new_thread->context.ecx;
	current_context->edx = new_thread->context.edx;
	current_context->esi = new_thread->context.esi;
	current_context->edi = new_thread->context.edi;
	current_context->eip = new_thread->context.eip;
	current_context->esp = new_thread->context.esp;
	current_context->eflags = new_thread->context.eflags;
}

void Scheduler::switch_page_directory(ProcessControlBlock* new_thread)
{
	Paging::load_page_directory(new_thread->page_directory);
}
