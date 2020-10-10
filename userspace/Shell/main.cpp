#include "String.h"
#include <ArgumentParser.h>
#include <Clib.h>
#include <ErrorCodes.h>
#include <PathView.h>
#include <systemlib/Systemcalls/Systemcalls.h>
#include <systemlib/Types.h>
#include <systemlib/iostream/iostream.h>

static String* working_directory;

enum class InternalCommands { cd, cwd, Invalid };

InternalCommands parse_internal_command(const ArgumentParser& arguments)
{
	if (arguments[0] == "cd") {
		return InternalCommands::cd;
	} else if (arguments[0] == "cwd") {
		return InternalCommands::cwd;
	} else {
		return InternalCommands::Invalid;
	}
}
void execute_internal_command(InternalCommands type, const ArgumentParser& arguments)
{
	switch (type) {
		case InternalCommands::cd:
			if (arguments.count() > 1) {
				*working_directory = arguments[1];
			} else {
				printf("you need to specify a path.\n");
			}
			break;
		case InternalCommands::cwd:
			printf(working_directory->c_str());
			break;
		default:
			break;
	}
}
void execute_external_command(const ArgumentParser& arguments)
{
	const char* args = nullptr;
	if (arguments.count() > 1) {
		args = arguments[1].data();
	}

	Handle child = CreateProcess(arguments[0].data(), args, 0);
	if (!child) {
		child = CreateProcess(PathView(working_directory->c_str(), arguments[0]).full_path().c_str(), args, 0);
		if (!child) {
			printf("Undefined command.");
			return;
		}
	}
	WaitSignal(child, 0);
	CloseHandle(child);
}

void execute_command(char* command)
{
	ArgumentParser arguments(command);
	if (arguments.count() == 0) {
		return;
	}
	InternalCommands com;
	if ((com = parse_internal_command(arguments)) != InternalCommands::Invalid) {
		execute_internal_command(com, arguments);
	} else {
		execute_external_command(arguments);
	}
}

int main(int argc, const char* argv[])
{
	working_directory = new String("/Tar/UserBinary");
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