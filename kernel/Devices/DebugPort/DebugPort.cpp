#include "DebugPort.h"
#include "Arch/x86/Asm.h"
#include "Lib/Stdlib.h"

void DebugPort::write(const char* data, size_t size, DebugColor color)
{
	char num[2];
	itoa(num, static_cast<int>(color), 10);
	put("\x1B[", 2);
	put(num, 2);
	put('m');
	put(data, size);
	put("\x1B[0m", 4);
}
void DebugPort::write(const char* data, DebugColor color)
{
	char num[2];
	itoa(num, static_cast<int>(color), 10);
	put("\x1B[", 2);
	put(num, 2);
	put('m');
	put(data, strlen(data));
	put("\x1B[0m", 4);
}

void DebugPort::put(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		out8(DEBUG_PORT, data[i]);
	}
}

void DebugPort::put(const char data)
{
	out8(DEBUG_PORT, data);
}