#include "scheduler.h"
#include "Arch/x86/atomic.h"
#include "Arch/x86/paging.h"
#include "Devices/Console/console.h"
#include "Devices/Null/null.h"
#include "VirtualMemory/heap.h"
#include "VirtualMemory/memory.h"

ThreadControlBlock* Scheduler::current_thread;
unsigned tid;

void hello1()
{
	for (size_t i = 0; i < 10; i++) {
		atomic_begin();
		printf("hello I'm thread 1, i love you.\n");
		atomic_end();
	}

	while (1) {
		/* code */
	}
}

void hello2()
{
	for (size_t i = 0; i < 10; i++) {
		atomic_begin();
		printf("hello I'm thread 2, i hate you.\n");
		atomic_end();
	}
	while (1) {
		/* code */
	}
}
void hello3()
{
	for (size_t i = 0; i < 10; i++) {
		atomic_begin();
		printf("hello I'm thread 3, i don't know you.\n");
		atomic_end();
	}
	while (1) {
		/* code */
	}
}

void Scheduler::setup()
{
	tid = 0;
	current_thread = 0;
	create_new_thread((uintptr_t)hello1);
	create_new_thread((uintptr_t)hello2);
	create_new_thread((uintptr_t)hello3);
}

void Scheduler::create_new_thread(uintptr_t address)
{
	ThreadControlBlock* new_thread = (ThreadControlBlock*)Heap::kmalloc(sizeof(ThreadControlBlock), 0);
	void* thread_stack = (void*)Memory::alloc(PAGE_SIZE, MEMORY_TYPE::KERNEL, MEMORY_TYPE::WRITABLE);
	new_thread->tid = tid++;
	new_thread->context.esp = (unsigned)thread_stack + PAGE_SIZE;
	new_thread->context.eip = address;
	new_thread->context.eflags = 0x200;
	new_thread->state = ThreadState::INTIALE;
	append_thread_list(new_thread);
}

void Scheduler::append_thread_list(ThreadControlBlock* new_thread)
{
	if (current_thread) {
		new_thread->next = current_thread;
		new_thread->prev = current_thread->prev;
		current_thread->prev->next = new_thread;
		current_thread->prev = new_thread;
	} else {
		new_thread->next = new_thread->prev = new_thread;
		current_thread = new_thread;
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

		if (thread_pointer->next == thread_pointer) {
			next_thread = thread_pointer;
			break;
		}

		if ((thread_pointer != current_thread) &&
		    ((thread_pointer->state == ThreadState::ACTIVE) || (thread_pointer->state == ThreadState::INTIALE)) &&
		    (!next_thread)) {
			next_thread = thread_pointer;
		}

		thread_pointer = thread_pointer->next;
	} while (thread_pointer != current_thread);

	if (next_thread->state == ThreadState::ACTIVE) {
		save_context(current_context);
		next_thread->state == ThreadState::RUNNING;
	}
	if (next_thread->state == ThreadState::INTIALE) {
		next_thread->state == ThreadState::RUNNING;
	}
	current_thread->state = ThreadState::ACTIVE;

	switch_context(current_context, next_thread);
	current_thread = next_thread;
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
	current_context->ebp = new_thread->context.ebp;
	current_context->eflags = new_thread->context.eflags;
}

void Scheduler::save_context(ContextFrame* current_context)
{
	current_thread->context.eax = current_context->eax;
	current_thread->context.ebx = current_context->ebx;
	current_thread->context.ecx = current_context->ecx;
	current_thread->context.edx = current_context->edx;
	current_thread->context.esi = current_context->esi;
	current_thread->context.edi = current_context->edi;
	current_thread->context.esp = current_context->esp;
	current_thread->context.ebp = current_context->ebp;
	current_thread->context.eip = current_context->eip;
	current_thread->context.eflags = current_context->eflags;
}

void Scheduler::switch_page_directory(ProcessControlBlock* new_thread)
{
	Paging::load_page_directory(new_thread->page_directory);
}
