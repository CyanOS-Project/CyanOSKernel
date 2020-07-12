#pragma once

#include "Filesystem/pipes/Pipe.h"
#include "Filesystem/ustar/INode.h"
#include "utils/ErrorCodes.h"
#include "utils/Result.h"
#include "utils/types.h"

#define MAX_OFFSET (SIZE_MAX / 4)
enum class SeekOrigin { SET, CURRENT, END };

class FileDescriptor
{
  private:
	FSNode& m_node;
	size_t m_current_position = 0;
	int m_type = 0;
	bool m_has_changed = false;

  public:
	FileDescriptor(FSNode& node);
	~FileDescriptor();
	Result<void> open();
	Result<void> close();
	Result<void> read(void* buff, size_t size);
	Result<void> write(void* buff, size_t size);
	Result<void> seek(int offset, SeekOrigin origin);
	Result<void> fstat();
	Result<void> ioctl();
	Result<void> mmap();
};