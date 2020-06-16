#pragma once
#include "utils/types.h"

#define JMP(x)               asm("JMP %0" : : "r"(kernel_init) :)
#define HLT()                asm("HLT")
#define DISABLE_INTERRUPTS() asm("CLI")
#define ENABLE_INTERRUPTS()  asm("STI")
#define SET_STACK(x)         asm("MOV %0,%%ESP; SUB $0x10,%%esp" : : "r"(x) :) // PUSH for the debuggers to work properly.

static inline uint8_t in8(uint16_t port)
{
	uint8_t data;
	asm volatile("inb %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline uint16_t in16(uint16_t port)
{
	uint16_t data;
	asm volatile("inw %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline uint32_t in32(uint16_t port)
{
	uint32_t data;
	asm volatile("inl %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline void out8(uint16_t port, uint8_t data)
{
	asm volatile("outb %0, %1" : : "a"(data), "d"(port));
}

static inline void out16(uint16_t port, uint16_t data)
{
	asm volatile("outw %0, %1" ::"a"(data), "d"(port));
}

static inline void out32(uint16_t port, uint32_t data)
{
	asm volatile("outl %0, %1" : : "a"(data), "d"(port));
}