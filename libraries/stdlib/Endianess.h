#pragma once
#include "Types.h"

constexpr uint8_t to_big_endian(uint8_t value)
{
	return value;
}

constexpr u16 to_big_endian(u16 value)
{
	return ((value & 0xFF00) >> 8) | ((value & 0xFF) << 8);
}

constexpr u32 to_big_endian(u32 value)
{
	return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) |
	       ((value & 0x000000FF) << 24);
}