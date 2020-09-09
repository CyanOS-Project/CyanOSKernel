//#include <libsystem/Systemcalls/Systemcalls.h>
//#include <libsystem/Types.h>
void execute_command(char* command) {}
extern "C" int main()
{
	/*HANDLE kb_fd = OpenFile("/Devices/keyboard", 0, 1);
	HANDLE con_fd = OpenFile("/Devices/console", 1, 1);
	WriteFile(con_fd, "ًWelcome To CyanOS\n", 21);
	WriteFile(con_fd, "ً$> ", 7);

	char c;
	const size_t max = 1000;
	char buff[max];
	size_t index = 0;
	while (ReadFile(kb_fd, &c, 1)) {
	    if (index < max) {
	        if (c == '\n') {
	            buff[index] = 0;
	            execute_command(buff);
	            WriteFile(con_fd, "ً\n$> ", 7);
	            index = 0;
	        } else {
	            buff[index++];
	            WriteFile(con_fd, &c, 1);
	        }
	    } else {
	        WriteFile(con_fd, "\ncommand is too long!", 22);
	        index = 0; // command is too long
	    }
	}
	CloseFile(kb_fd);
	while (1) {
	} */
	return 0;
}