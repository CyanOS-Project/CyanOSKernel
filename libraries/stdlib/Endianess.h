#pragma once
#include "Types.h"

template <typename T> constexpr T to_big_endian(size_t value);

template <> constexpr u8 to_big_endian(size_t value)
{
	return value;
}

template <> constexpr u16 to_big_endian(size_t value)
{
	return ((value & 0xFF00) >> 8) | ((value & 0xFF) << 8);
}

template <> constexpr u32 to_big_endian(size_t value)
{
	return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) |
	       ((value & 0x000000FF) << 24);
}