#pragma once
#include "boot/multiboot2.h"
#include <types.h>

struct Module {
	void* start;
	size_t size;
};

struct BootloaderInfo {
	Module ramdisk;
};

extern "C" void kernel_init(BootloaderInfo* boot_info);
void call_constrcutors();
void idle();