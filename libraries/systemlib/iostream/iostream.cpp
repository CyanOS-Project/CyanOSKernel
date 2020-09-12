#include "iostream.h"
#include "../Systemcalls/Systemcalls.h"
#include <Clib.h>

HANDLE con_fd = -1;
HANDLE kb_fd = -1;

void printf(const char* str)
{
	if (con_fd == HANDLE(-1)) {
		con_fd = OpenFile("/Devices/console", 1, 1);
		if (!con_fd) {
			// crash here
		}
	}

	if (!WriteFile(con_fd, str, strlen(str))) {
		// crash here
	}
}

char get_char()
{
	char c = 0;
	if (kb_fd == HANDLE(-1)) {
		kb_fd = OpenFile("/Devices/keyboard", 0, 1);
		if (!kb_fd) {
			// crash here
		}
	}

	if (ReadFile(kb_fd, &c, 1) == 1) {
		// crash here
	}
	return c;
}

void get_line(char* buff, size_t size)
{
	UNUSED(buff);
	UNUSED(size);
}