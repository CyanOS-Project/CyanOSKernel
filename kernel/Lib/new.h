#pragma once

#include "VirtualMemory/heap.h"
#include "utils/types.h"
inline void* operator new(size_t, void* p)
{
	return p;
}

inline void* operator new[](size_t, void* p)
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