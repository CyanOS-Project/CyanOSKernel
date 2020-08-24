#pragma once
#include "Utils/Types.h"

struct Module {
	uintptr_t start;
	uintptr_t size;
};
struct BootloaderInfo {
	Module ramdisk;
};

extern "C" void kernel_init(BootloaderInfo* boot_info);