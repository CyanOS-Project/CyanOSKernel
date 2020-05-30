#include "asm.h"
#include "console.h"

#define PANIC(str)                                                                                                     \
	do {                                                                                                               \
		clearScreen();                                                                                                 \
		printf("PANIC: %s in %s:%d\n", str, __FILE__, __LINE__);                                                       \
		HLT();                                                                                                         \
	} while (0)
