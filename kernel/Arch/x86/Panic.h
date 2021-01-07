#pragma once

#include "Asm.h"
#include "Devices/DebugPort/Logger.h"

#define PANIC(str)                                                                                                     \
	do {                                                                                                               \
		DISABLE_INTERRUPTS();                                                                                          \
		HLT();                                                                                                         \
	} while (0)
