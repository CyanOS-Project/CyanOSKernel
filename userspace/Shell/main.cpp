#include <Clib.h>
#include <systemlib/Systemcalls/Systemcalls.h>
#include <systemlib/Types.h>
#include <systemlib/iostream/iostream.h>

void execute_command(char* command)
{
	if (strcmp(command, "TestApp") == 0) {
		Handle child = CreateProcess("TestApp", "/Tar/UserBinary/TestApp", 0);
		WaitSignal(child, 0);
	} else {
		printf("Undefined command.\n"); // TODO: do some cute error messages here.
	}
}

int main()
{
	printf("Welcome To CyanOS\n");
	printf("Version 0.1 Alpha\n");
	printf("$> ");

	char c;
	const size_t max = 1000;
	char buff[max];
	size_t index = 0;
	while ((c = get_char())) {
		if (index < max) {
			if (c == '\n') {
				buff[index] = 0;
				printf("\n");
				execute_command(buff);
				printf("$> ");
				index = 0;
			} else if (c == '\b') {
				if (index > 0) {
					index--;
					printf("\b");
				} else { // Beep or something
				}
			} else {
				buff[index++] = c;
				printf(&c);
			}
		} else {
			printf("\ncommand is too long!");
			index = 0; // command is too long
		}
	}
	while (1) {
	}
	return 0;
}