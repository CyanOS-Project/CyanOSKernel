#pragma once
#include "Boot/Multiboot2.h"
#include <Types.h>

struct Module {
	uintptr_t start;
	uintptr_t size;
};

struct BootloaderInfo {
	multiboot_tag_start* tags;
	Module ramdisk;
};

extern "C" void kernel_init(BootloaderInfo* boot_info);
void call_constrcutors();
void idle(uintptr_t);