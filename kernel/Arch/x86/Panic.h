#pragma once

#include "Asm.h"
#include "Devices/DebugPort/Logger.h"

#define PANIC(str)                                                                                                     \
	do {                                                                                                               \
		err() << "PANIC: " << str << " " << __FILE__ << ":" << __LINE__;                                               \
		HLT();                                                                                                         \
	} while (0)
