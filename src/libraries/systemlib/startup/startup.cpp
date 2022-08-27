
#include "../systemcalls/systemcalls.h"
#include "../types.h"
#include "kernel/tasking/process_definitions.h"

extern "C" int main(int argc, const char* argv[]);

const char* executable_path()
{
	char* path;
	asm("movl %%gs:(%1),%0" : "=r"(path) : "r"(offsetof(UserProcessInformationBlock, path)));
	return path;
}

const char* program_arguments()
{
	char* arg;
	asm("movl %%gs:(%1),%0" : "=r"(arg) : "r"(offsetof(UserProcessInformationBlock, argument)));
	return arg;
}

const uintptr_t* constructors_array()
{
	const uintptr_t* constructors_array;
	asm("movl %%gs:(%1),%0"
	    : "=r"(constructors_array)
	    : "r"(offsetof(UserProcessInformationBlock, constructors_array)));
	return constructors_array;
}

size_t constructors_array_count()
{
	size_t constructors_array_count;
	asm("movl %%gs:(%1),%0"
	    : "=r"(constructors_array_count)
	    : "r"(offsetof(UserProcessInformationBlock, constructors_array_count)));
	return constructors_array_count;
}

void call_constructors()
{
	typedef void (*func)(void);
	size_t count = constructors_array_count();
	const uintptr_t* constructors = constructors_array();
	for (size_t i = 0; i < count; i++) {
		if (*constructors && *constructors != uintptr_t(-1)) {
			func constructor = (func)*constructors;
			constructor();
			constructors++;
		}
	}
}

extern "C" int startup()
{
	call_constructors();

	const char* main_args[2];
	main_args[0] = executable_path();
	main_args[1] = program_arguments();
	main(2, main_args);
	TerminateProcess(-1, 0);
	return 1;
}