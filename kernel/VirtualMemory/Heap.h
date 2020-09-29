#pragma once
#include "SlabAllocator.h"
#include <Types.h>

class Heap
{
  private:
	static Slab<0x010, PAGE_SIZE> slab_0x10;
	static Slab<0x020, PAGE_SIZE> slab_0x20;
	static Slab<0x040, PAGE_SIZE> slab_0x40;
	static Slab<0x080, PAGE_SIZE> slab_0x80;
	static Slab<0x100, PAGE_SIZE> slab_0x100;
	static Slab<0x200, PAGE_SIZE> slab_0x200;
	static Slab<0x300, PAGE_SIZE> slab_0x300;

  public:
	static void setup();
	static void* kmalloc(size_t size);
	static void kfree(void* address);
};