#pragma once
#include "Utils/Types.h"

#define JMP(x)               asm("JMP *%0" : : "r"(kernel_init) :)
#define CALL(x, arg)         asm volatile("pushl %0;call *(%1);" : : "r"(arg), "r"(x));
#define HLT()                asm("HLT")
#define DISABLE_INTERRUPTS() asm("CLI")
#define ENABLE_INTERRUPTS()  asm("STI")
#define SET_STACK(x)         asm("MOVL %%ESP,%0; SUB %%esp,0x10" : : "r"(x) :) // PUSH for the debuggers to work properly.

static inline uint32_t get_faulted_page()
{
	uint32_t page;
	asm("MOV %0,%%CR2" : "=r"(page));
	return page;
}

static inline uint8_t in8(uint16_t port)
{
	uint8_t data;
	asm volatile("inb %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline uint16_t in16(uint16_t port)
{
	uint16_t data;
	asm volatile("inw %0, %1" : "=a"(data) : "d"(port));
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

static inline uint32_t test_and_set(volatile uint32_t* address)
{
	uint32_t result;
	asm volatile("LOCK xchg %1, %0\n" : "+m"(*address), "=a"(result) : "1"(1) : "cc", "memory");
	return result;
}

static inline uint32_t eflags_read()
{
	uint32_t eflags;
	asm volatile("PUSHF\n"
	             "POP %0\n"
	             : "=r"(eflags));

	return eflags;
}

static inline void eflags_write(uint32_t eflags)
{
	asm volatile("PUSHL %0\n"
	             "POPF \n" ::"r"(eflags));
}