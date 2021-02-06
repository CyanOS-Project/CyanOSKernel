#pragma once
#include "Types.h"

constexpr uint8_t to_big_endian(uint8_t value)
{
	return value;
}

constexpr uint16_t to_big_endian(uint16_t value)
{
	return ((value & 0xFF00) >> 8) | ((value & 0xFF) << 8);
}

constexpr uint32_t to_big_endian(uint32_t value)
{
	return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) |
	       ((value & 0x000000FF) << 24);
}