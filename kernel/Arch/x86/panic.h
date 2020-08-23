#pragma once

#include "asm.h"

#define PANIC() HLT();

// printf("PANIC: %s in %s:%d\n", str, __FILE__, __LINE__);