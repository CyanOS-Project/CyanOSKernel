#pragma once
#include <Types.h>

struct UserProcessInformationBlock {
	size_t pid;
	const char* path;
	const char* argument;
	char path_data[MAX_PATH_SIZE];
	char argument_data[MAX_PATH_SIZE];
	const uintptr_t* constructors_array;
	size_t constructors_array_count;
};