#include "Heap.h"
#include "Arch/x86/Panic.h"

Slab<0x010, PAGE_SIZE> Heap::slab_0x10;
Slab<0x020, PAGE_SIZE> Heap::slab_0x20;
Slab<0x040, PAGE_SIZE> Heap::slab_0x40;
Slab<0x080, PAGE_SIZE> Heap::slab_0x80;
Slab<0x100, PAGE_SIZE> Heap::slab_0x100;
// Slab<0x200, PAGE_SIZE> Heap::slab_0x200;
// Slab<0x300, PAGE_SIZE> Heap::slab_0x300;

void Heap::setup()
{
	slab_0x10.init();
	slab_0x20.init();
	slab_0x40.init();
	slab_0x80.init();
	slab_0x100.init();
}

void* Heap::kmalloc(size_t size)
{
	if (!size)
		return nullptr;

	if (size <= 0x10) {
		return slab_0x10.alloc();
	} else if (size <= 0x20) {
		return slab_0x20.alloc();
	} else if (size <= 0x40) {
		return slab_0x40.alloc();
	} else if (size <= 0x80) {
		return slab_0x80.alloc();
	} else if (size <= 0x100) {
		return slab_0x100.alloc();
	} else {
		return BigSlabAllocator::alloc(size);
	}
}

void Heap::kfree(void* address)
{
	if (!address)
		return;

	auto* block = reinterpret_cast<MemoryBlock<>*>(static_cast<char*>(address) - offsetof(MemoryBlock<>, data));
	ASSERT(block);
	ASSERT(block->slab_ptr);
	switch (block->slab_size) {
		case 0x10: {
			auto* slab = static_cast<Slab<0x10, PAGE_SIZE>*>(block->slab_ptr);
			slab->free(address);
			break;
		}
		case 0x20: {
			auto* slab = static_cast<Slab<0x20, PAGE_SIZE>*>(block->slab_ptr);
			slab->free(address);
			break;
		}
		case 0x40: {
			auto* slab = static_cast<Slab<0x40, PAGE_SIZE>*>(block->slab_ptr);
			slab->free(address);
			break;
		}
		case 0x80: {
			auto* slab = static_cast<Slab<0x80, PAGE_SIZE>*>(block->slab_ptr);
			slab->free(address);
			break;
		}
		case 0x100: {
			auto* slab = static_cast<Slab<0x100, PAGE_SIZE>*>(block->slab_ptr);
			slab->free(address);
			break;
		}
		default:
			auto* slab = static_cast<BigSlab*>(block->slab_ptr);
			BigSlabAllocator::free(slab);
	}
}

void* operator new(size_t size)
{
	return Heap::kmalloc(size);
}

void* operator new[](size_t size)
{
	return Heap::kmalloc(size);
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

/*extern "C" void __cxa_pure_virtual()
{
    PANIC("Virtual function has no implementation!");
}*/

extern "C" void atexit()
{
	PANIC("Exiting!");
}
