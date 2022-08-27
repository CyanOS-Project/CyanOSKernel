#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint-gcc.h>

#define _UNUSED_PARAM(x) x tmp __attribute__((__unused__))
#define UNUSED(expr)     (void)(expr)
#define UNUSEDARG        __attribute__((unused))

#define Bit(shift)         (1 << shift)
#define Bits(value, shift) (value << shift)

using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using uptr = uintptr_t;