#pragma once
#include "Types.h"

struct Module {
	uintptr_t start;
	uintptr_t size;
};
struct BootloaderInfo {
	Module ramdisk;
};

extern "C" void kernel_init(BootloaderInfo* boot_info);