#pragma once
#include <types.h>

#define CALL(x, arg)         asm volatile("pushl %0;call *%1;" : : "r"(arg), "r"(x));
#define HLT()                asm volatile("hlt")
#define DISABLE_INTERRUPTS() asm volatile("cli")
#define ENABLE_INTERRUPTS()  asm volatile("sti")
#define SET_STACK(x)         asm volatile("movl %0, %%esp; sub $0x10, %%esp" : : "r"(x) :)

static inline u32 get_faulted_page()
{
	u32 page;
	asm volatile("mov %%cr2, %0" : "=r"(page));
	return page;
}

static inline uint8_t in8(u16 port)
{
	uint8_t data;
	asm volatile("inb %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline u16 in16(u16 port)
{
	u16 data;
	asm volatile("inw %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline u32 in32(u16 port)
{
	u32 data;
	asm volatile("inl %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline void out8(u16 port, uint8_t data)
{
	asm volatile("outb %0, %1" : : "a"(data), "d"(port));
}

static inline void out16(u16 port, u16 data)
{
	asm volatile("outw %0, %1" ::"a"(data), "d"(port));
}

static inline void out32(u16 port, u32 data)
{
	asm volatile("outl %0, %1" : : "a"(data), "d"(port));
}

static inline u32 test_and_set(volatile u32* address)
{
	u32 result;
	asm volatile("lock xchg %1, %0\n" : "+m"(*address), "=a"(result) : "1"(1) : "cc", "memory");
	return result;
}

static inline u32 eflags_read()
{
	u32 eflags;
	asm volatile("pushf\n"
	             "popl %0\n"
	             : "=r"(eflags));

	return eflags;
}

static inline void eflags_write(u32 eflags)
{
	asm volatile("pushl %0\n"
	             "popf \n" ::"r"(eflags));
}