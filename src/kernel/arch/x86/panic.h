#pragma once

#include "asm.h"
#include "devices/debug_port/logger.h"

#define PANIC(str)                                                                                                     \
	do {                                                                                                               \
		DISABLE_INTERRUPTS();                                                                                          \
		err() << "PANIC: " << str << " " << __FILE__ << ":" << __LINE__;                                               \
		HLT();                                                                                                         \
	} while (0)
