#include "malloc.h"
#include "VirtualMemory/heap.h"
#include <stddef.h>

/*void* operator new(size_t size)
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
*/
void* operator new(size_t, void* p)
{
	return p;
}

void* operator new[](size_t, void* p)
{
	return p;
}

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

void operator delete(void* p, size_t)
{
	return Heap::kfree(p);
}

void operator delete[](void* p)
{
	return Heap::kfree(p);
}

void operator delete[](void* p, size_t)
{
	return Heap::kfree(p);
}