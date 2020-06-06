#pragma once
#include "types.h"

#define JMP(x)       asm("JMP %0" : : "r"(kernel_init) :)
#define HLT()        asm("HLT")
#define CLI()        asm("CLI")
#define STI()        asm("STI")
#define SET_STACK(x) asm("MOV %0,%%ESP; SUB $0x10,%%esp" : : "r"(x) :) // PUSH for the debuggers to work properly.

static inline uint8_t in8(uint16_t port)
{
	uint8_t data;
	asm("MOVW %1, %%DX;"
	    "IN %%DX, %%AL;"
	    "MOVB %%AL, %0;"
	    : "=r"(data)
	    : "r"(port)
	    : "dx");
	return data;
}

static inline void out8(uint16_t port, uint8_t data)
{
	asm("MOVW %0, %%DX;"
	    "MOVB %1, %%AL;"
	    "OUT %%AL, %%DX;"
	    :
	    : "r"(port), "r"(data)
	    : "dx", "al");
}
