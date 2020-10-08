#include <ArgumentParser.h>
#include <Clib.h>
#include <ErrorCodes.h>
#include <PathView.h>
#include <systemlib/Systemcalls/Systemcalls.h>
#include <systemlib/Types.h>
#include <systemlib/iostream/iostream.h>

static const char* bin_dir = "/Tar/UserBinary";

void execute_command(char* command)
{
	ArgumentParser arguments(command);
	if (arguments.count() == 0) {
		return;
	}

	const char* args = nullptr;
	if (arguments.count() > 1) {
		args = arguments[1].data();
	}

	Handle child = CreateProcess(arguments[0].data(), args, 0);
	if (!child) {
		child = CreateProcess(PathView(bin_dir, arguments[0]).full_path().c_str(), args, 0);
		if (!child) {
			printf("Undefined command.");
			return;
		}
	}
	WaitSignal(child, 0);
	CloseHandle(child);
}

int main(int argc, const char* argv[])
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
				printf("\n$> ");
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