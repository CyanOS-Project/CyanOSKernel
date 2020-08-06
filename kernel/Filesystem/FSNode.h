#pragma once

#include "utils/Result.h"
#include "utils/types.h"

class FSNode
{
  private:
  public:
	int m_permission;
	int m_owner;
	size_t m_size;

	virtual Result<void> read(void* buff, size_t offset, size_t size) = 0;
	virtual Result<void> write(void* buff, size_t offset, size_t size) = 0;
	virtual Result<bool> can_read() = 0;
	virtual Result<bool> can_write() = 0;
	virtual Result<void> remove() = 0;
	virtual Result<void> create(const char* name, void* info) = 0;
	virtual Result<void> mkdir(const char* name, void* info) = 0;
	virtual Result<void> link(FSNode& node) = 0;
	virtual Result<void> unlink(FSNode& node) = 0;
	virtual Result<FSNode&> dir_lookup(const char* file_name) = 0;
};

#define MAX_FILE_NAME 64
