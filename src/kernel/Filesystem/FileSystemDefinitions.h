#pragma once

#include <Types.h>

#define MAX_FILE_NAME 64
#define MAX_PATH_SIZE 512
#define MAX_ARG_SIZE  512

enum class NodeType
{
	Root,
	Folder,
	File,
	Pipe,
	Link,
	Device,
	DomainSocket,
	IPSocket,
	Connection
};
enum OpenFlags
{
	OF_OPEN_EXISTING = Bit(0),
	OF_CREATE_NEW = Bit(1),
};
enum OpenMode
{
	OM_READ = Bit(0),
	OM_WRITE = Bit(1),
	OM_SERVER = Bit(2),
	OM_CLIENT = Bit(3),
};

enum class SeekOrigin
{
	SET,
	CURRENT,
	END
};
struct FileInfo {
	char file_name[MAX_FILE_NAME];
	size_t size;
	NodeType type;
};