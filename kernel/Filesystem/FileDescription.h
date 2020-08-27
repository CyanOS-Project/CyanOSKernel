#pragma once

#include "FSNode.h"
#include "Utils/Result.h"
#include "Utils/Types.h"

enum class SeekOrigin { SET, CURRENT, END };
struct FileInfo {
	size_t size;
};

class FileDescription
{
  private:
	FSNode& m_node;
	size_t m_current_position = 0;
	bool m_has_changed = false;

  public:
	const NodeType m_type;
	const OpenMode m_mode;

	FileDescription(FSNode& node, OpenMode mode);
	~FileDescription();
	Result<void> read(void* buff, size_t size);
	Result<void> write(const void* buff, size_t size);
	Result<void> seek(int offset, SeekOrigin origin);
	Result<FileInfo> fstat();
	Result<void> ioctl();
	Result<void> mmap();
};