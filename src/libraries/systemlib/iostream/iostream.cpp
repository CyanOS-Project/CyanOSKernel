#include "iostream.h"
#include "../systemcalls/systemcalls.h"
#include <clib.h>

Handle con_fd = -1;
Handle kb_fd = -1;

void printf(const char* str)
{
	if (con_fd == Handle(-1)) {
		con_fd = OpenFile("/devices/console", 1, 1);
		if (!con_fd) {
			// crash here
		}
	}

	if (!WriteFile(con_fd, str, strlen(str))) {
		// crash here
	}
}

void putchar(char c)
{
	if (con_fd == Handle(-1)) {
		con_fd = OpenFile("/devices/console", 1, 1);
		if (!con_fd) {
			// crash here
		}
	}

	if (!WriteFile(con_fd, &c, 1)) {
		// crash here
	}
}

char get_char()
{
	char c = 0;
	if (kb_fd == Handle(-1)) {
		kb_fd = OpenFile("/devices/keyboard", 0, 1);
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