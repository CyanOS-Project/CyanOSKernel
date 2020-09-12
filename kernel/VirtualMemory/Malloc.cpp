#include "Arch/x86/Panic.h"
#include "Types.h"
#include "VirtualMemory/Heap.h"

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

void* operator new(size_t, void* p)
{
	return p;
}

void* operator new[](size_t, void* p)
{
	return p;
}

void operator delete[](void* p, size_t)
{
	Heap::kfree(p);
}
void operator delete(void* p, size_t)
{
	Heap::kfree(p);
}

extern "C" void __cxa_pure_virtual()
{
	PANIC("Virtual function has no implementation!");
}