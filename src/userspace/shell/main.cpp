#include "main.h"
#include "string.h"
#include <clib.h>
#include <error_codes.h>
#include <path_view.h>
#include <systemlib/types.h>
#include <systemlib/iostream/iostream.h>

static String working_directory{""};

int main(int argc, const char* argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	working_directory = "/tar/bin";
	printf("Welcome To CyanOS\n");
	printf("Version 0.1 Alpha\n");
	printf(working_directory.c_str());
	printf(" $> ");

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
				printf(working_directory.c_str());
				printf(" $> ");
				index = 0;
			} else if (c == '\b') {
				if (index > 0) {
					index--;
					printf("\b");
				} else { // Beep or something
				}
			} else {
				buff[index++] = c;
				putchar(c);
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

InternalCommands parse_internal_command(const ArgumentParser& arguments)
{
	if (arguments[0] == "cd") {
		return InternalCommands::cd;
	} else if (arguments[0] == "cwd") {
		return InternalCommands::cwd;
	} else if (arguments[0] == "ls") {
		return InternalCommands::ls;
	} else {
		return InternalCommands::Invalid;
	}
}
void execute_internal_command(InternalCommands type, const ArgumentParser& arguments)
{
	switch (type) {
		case InternalCommands::cd:
			command_cd(arguments);
			break;
		case InternalCommands::cwd:
			command_cwd();
			break;
		case InternalCommands::ls:
			command_ls(arguments);
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
		child = CreateProcess(PathView(working_directory.c_str(), arguments[0]).full_path().c_str(), args, 0);
		if (!child) {
			printf("Undefined command.\n");
			return;
		}
	}
	WaitSignal(child, 0);
	CloseHandle(child);
}

void command_cd(const ArgumentParser& arguments)
{
	if (arguments.count() < 2) {
		printf("You need to specify a path.\n");
		return;
	}

	String new_path = [&]() {
		if (arguments[1] == ".." || arguments[1] == "-") {
			auto cwd_view = PathView(working_directory);
			if (cwd_view.count() <= 1) {
				return working_directory;
			} else {
				return cwd_view.sub_path(0, -2).full_path();
			}
		} else if (PathView::get_type(arguments[1]) == PathType::Relative) {
			return PathView(working_directory, arguments[1].data()).full_path();
		} else {
			return String(arguments[1]);
		}
	}();

	Handle dummy = OpenFile(new_path.c_str(), OM_READ, OF_OPEN_EXISTING);
	if (GetLastError()) {
		printf("Cannot access this directory.\n");
	} else {
		FileInfo info;
		QueryFileInformation(dummy, &info);
		if (GetLastError()) {
			printf("Cannot access this directory.\n");
		} else if (info.type != NodeType::Folder && info.type != NodeType::Root) {
			printf("Path is not direcotry.\n");
		} else {
			working_directory = new_path;
		}
		CloseHandle(dummy);
	}
}

void command_cwd()
{
	printf(working_directory.c_str());
	printf("\n");
}

void command_ls(const ArgumentParser& arguments)
{
	Handle fd;
	if (arguments.count() > 1) {
		fd = OpenFile(arguments[1].data(), OM_READ, OF_OPEN_EXISTING);
	} else {
		fd = OpenFile(working_directory.c_str(), OM_READ, OF_OPEN_EXISTING);
	}

	if (GetLastError()) {
		printf("Cannot open the directory.\n");
	}

	FileInfo info;
	QueryDirectory(fd, &info);
	while (!GetLastError()) {
		printf(info.file_name);
		printf("\n");
		QueryDirectory(fd, &info);
	}
	CloseHandle(fd);
}

/*
man
ps
free
echo
find
kill
x! (previous commands)
*/