#pragma once

#include <Types.h>

#define MAX_FILE_NAME 64
#define MAX_PATH_SIZE 512
#define MAX_ARG_SIZE  512

enum class NodeType { Root, Folder, File, Pipe, Link, Device, Socket, Connection };
enum OpenFlags {
	OF_OPEN_EXISTING = MASKABLE_PRAM(0),
	OF_CREATE_NEW = MASKABLE_PRAM(1),
};
enum OpenMode {
	OM_READ = MASKABLE_PRAM(0),
	OM_WRITE = MASKABLE_PRAM(1),
	OM_SERVER = MASKABLE_PRAM(2),
	OM_CLIENT = MASKABLE_PRAM(3),
};

enum class SeekOrigin { SET, CURRENT, END };
struct FileInfo {
	size_t size;
	NodeType type;
};

struct DirectoryInfo {
	char file_name[MAX_FILE_NAME];
	NodeType type;
};
