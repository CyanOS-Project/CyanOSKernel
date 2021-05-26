#pragma once

#include "FSNode.h"
#include "Network/NetworkDefinitions.h"
#include <BufferMutableView.h>
#include <BufferView.h>
#include <PathView.h>
#include <Result.h>
#include <Rule5.h>
#include <Types.h>
#include <UniquePointer.h>

class FileDescription
{
  public:
	NON_MOVABLE(FileDescription)
	NON_COPYABLE(FileDescription)

	const NodeType m_type;
	const OpenMode m_mode;

	static Result<UniquePointer<FileDescription>> open(PathView path, OpenMode mode, OpenFlags flags);
	static Result<UniquePointer<FileDescription>> socket(SocketAddress addr, SocketProtocol protocol, size_t flags);
	FileDescription(FSNode& node, OpenMode mode);
	~FileDescription();
	Result<size_t> read(BufferMutableView);
	Result<size_t> write(BufferView);
	Result<void> connect();
	Result<UniquePointer<FileDescription>> accept();
	Result<void> seek(int offset, SeekOrigin origin);
	Result<void> ioctl();
	Result<void> mmap();
	Result<void> dir_query(FileInfo& info);
	Result<void> file_query(FileInfo& file);

  private:
	FSNode& m_node;
	size_t m_current_position = 0;
	// bool m_has_changed = false;
};