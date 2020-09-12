#pragma once
#include "StringView.h"
#include "Types.h"

enum class DebugColor {
	Black = 30,
	Red = 31,
	Green = 32,
	Yellow = 33,
	Blue = 34,
	Magenta = 35,
	Cyan = 36,
	White = 37,
	Bright_Black = 90,
	Bright_Red = 91,
	Bright_Green = 92,
	Bright_Yellow = 93,
	Bright_Blue = 94,
	Bright_Magenta = 95,
	Bright_Cyan = 96,
	Bright_White = 97
};

class DebugPort
{
	constexpr static size_t DEBUG_PORT = 0x3F8;

  public:
	static void write(const char* data, size_t size, DebugColor color = DebugColor::Black);
	static void write(const char* data, DebugColor color = DebugColor::Black);
	static void put(const char* data, size_t size);
	static void put(const char data);
};
