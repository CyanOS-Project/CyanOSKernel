#pragma once
#include "Boot/Multiboot2.h"
#include <Types.h>

struct Module {
	void* start;
	size_t size;
};

struct BootloaderInfo {
	Module ramdisk;
};

extern "C" void kernel_init(BootloaderInfo* boot_info);
void call_constrcutors();
void idle(uintptr_t);