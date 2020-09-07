#include <libsystem/Systemcalls/Systemcalls.h>
#include <libsystem/Types.h>
void execute_command(char* command) {}
void main()
{
	HANDLE kb_fd = OpenFile("/Devices/keyboard", 0, 1);
	HANDLE con_fd = OpenFile("/Devices/console", 1, 1);

	char c;
	const size_t max = 1000;
	char buff[max];
	size_t index = 0;
	while (ReadFile(kb_fd, &c, 1)) {
		if (index < max) {
			buff[index++];
		} else if (c == '\n') {
			buff[index] = 0;
			execute_command(buff);
			index = 0;
		} else {
			WriteFile(con_fd, "\ncommand is too long!", 22);
			index = 0; // command is too long
		}

		WriteFile(con_fd, &c, 1);
	}
	CloseFile(kb_fd);
	while (1) {
		/* code */
	}
}