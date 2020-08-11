#pragma once

#include "utils/Result.h"
#include "utils/types.h"

enum class OpenFlags { CreateNew, OpenExisting };
enum class OpenMode { Read, Write, ReadWrite };

class FSNode
{
  private:
  public:
	int m_permission;
	int m_owner;
	size_t m_size;

	virtual Result<FSNode&> create(const char* name, OpenMode mode, OpenFlags flags) = 0;
	virtual Result<void> read(void* buff, size_t offset, size_t size) = 0;
	virtual Result<void> write(void* buff, size_t offset, size_t size) = 0;
	virtual Result<bool> can_read() = 0;
	virtual Result<bool> can_write() = 0;
	virtual Result<void> remove() = 0;
	virtual Result<void> mkdir(const char* name, int flags, int access) = 0;
	virtual Result<void> link(FSNode& node) = 0;
	virtual Result<void> unlink(FSNode& node) = 0;
	virtual Result<FSNode&> dir_lookup(const char* file_name) = 0;
};

#define MAX_FILE_NAME 64
