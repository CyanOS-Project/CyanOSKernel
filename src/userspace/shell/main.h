#pragma once

#include <argument_parser.h>
#include <systemlib/systemcalls/systemcalls.h>

enum class InternalCommands { cd, cwd, ls, Invalid };

void execute_command(char* command);
InternalCommands parse_internal_command(const ArgumentParser& arguments);
void execute_internal_command(InternalCommands type, const ArgumentParser& arguments);
void execute_external_command(const ArgumentParser& arguments);

void command_cd(const ArgumentParser& arguments);
void command_cwd();
void command_ls(const ArgumentParser& arguments);