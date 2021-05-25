#pragma once

#include "FileSystemDefinitions.h"
#include <BufferMutableView.h>
#include <BufferView.h>
#include <ErrorCodes.h>
#include <Result.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>

class VirtualFileSystem;
class FileDescription;

class FSNode
{
  public:
	String m_name;
	int m_permission;
	int m_owner;
	NodeType m_type;
	size_t m_size;

	FSNode(const StringView& name, int permission, int owner, NodeType type, size_t size) :
	    m_name{name},
	    m_permission{permission},
	    m_owner{owner},
	    m_type{type},
	    m_size{size}
	{
	}

#define INVALID_OP return ResultError(ERROR_INVALID_OPERATION)

#pragma GCC diagnostic ignored "-Wunused-parameter"
	virtual ~FSNode() = default;
	virtual Result<void> open(FileDescription&) { INVALID_OP; }
	virtual Result<void> close(FileDescription&) { INVALID_OP; }
	virtual bool can_read(FileDescription&) { return false; }
	virtual bool can_write(FileDescription&) { return false; }
	virtual bool can_accept() { return false; }
	virtual bool is_connected() { return false; }
	virtual Result<void> remove() { INVALID_OP; }
	virtual Result<void> link(FSNode& node) { INVALID_OP; }
	virtual Result<void> unlink(FSNode& node) { INVALID_OP; }
	virtual Result<void> connect() { INVALID_OP; }
	virtual Result<void> listen() { INVALID_OP; }
	virtual Result<FSNode&> accept() { INVALID_OP; }
	virtual Result<size_t> read(FileDescription&, BufferMutableView dest, size_t offset) { INVALID_OP; }
	virtual Result<size_t> write(FileDescription&, BufferView src, size_t offset) { INVALID_OP; }
	virtual Result<FSNode&> dir_lookup(const StringView& file_name) { INVALID_OP; }
	virtual Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) { INVALID_OP; }
	virtual Result<void> mkdir(const StringView& name, int flags, int access) { INVALID_OP; }
	virtual Result<FSNode&> dir_query(size_t index) { INVALID_OP; }
#pragma GCC diagnostic pop

	friend class VFS;
};
