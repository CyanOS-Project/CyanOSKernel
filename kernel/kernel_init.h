#pragma once
#include "utils/types.h"

struct Module {
	uintptr_t start;
	uintptr_t size;
};
struct BootloaderInfo {
	Module modules[1];
};

extern "C" void kernel_init(BootloaderInfo* info);