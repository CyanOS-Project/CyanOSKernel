#pragma once

#include "Devices/Console/console.h"
#include "asm.h"

#define PANIC(str)                                                                                                     \
	do {                                                                                                               \
		printf("PANIC: %s in %s:%d\n", str, __FILE__, __LINE__);                                                       \
		HLT();                                                                                                         \
	} while (0)
