#pragma once

#include "utils/Result.h"
#include "utils/StringView.h"
#include "utils/types.h"

enum class OpenFlags { CreateNew, OpenExisting };
enum class OpenMode { Read, Write, ReadWrite };
class VirtualFileSystem;

class FSNode
{
  protected:
	enum class NodeType { Root, Folder, File, Link };

  public:
	int m_permission;
	int m_owner;
	NodeType m_type;
	size_t m_size;
	FSNode(int permission, int owner, NodeType type, size_t size) :
	    m_permission{permission},
	    m_owner{owner},
	    m_type{type},
	    m_size{size} {};
	virtual Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) = 0;
	virtual Result<void> read(void* buff, size_t offset, size_t size) = 0;
	virtual Result<void> write(const void* buff, size_t offset, size_t size) = 0;
	virtual Result<bool> can_read() = 0;
	virtual Result<bool> can_write() = 0;
	virtual Result<void> remove() = 0;
	virtual Result<void> mkdir(const StringView& name, int flags, int access) = 0;
	virtual Result<void> link(FSNode& node) = 0;
	virtual Result<void> unlink(FSNode& node) = 0;
	virtual Result<FSNode&> dir_lookup(const StringView& file_name) = 0;

	friend class VFS;
};

#define MAX_FILE_NAME 64
