#pragma once

#include "Devices/DebugPort/Logger.h"
#include "asm.h"

#define PANIC(str)                                                                                                     \
	do {                                                                                                               \
		error() << "PANIC: " << str << " " << __FILE__ << ":" << __LINE__ << "\n";                                     \
		HLT();                                                                                                         \
	} while (0)
