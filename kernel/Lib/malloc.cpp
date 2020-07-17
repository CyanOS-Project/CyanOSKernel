#include "Arch/x86/panic.h"
#include "VirtualMemory/heap.h"
#include "utils/types.h"

void* operator new(size_t size)
{
	return Heap::kmalloc(size, 0);
}

void* operator new[](size_t size)
{
	return Heap::kmalloc(size, 0);
}

void operator delete(void* p)
{
	return Heap::kfree(p);
}

void operator delete[](void* p)
{
	return Heap::kfree(p);
}

extern "C" void __cxa_pure_virtual()
{
	PANIC("Virtual function has no implementation!");
}