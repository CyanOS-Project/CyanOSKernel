#pragma once

#include "Utils/ErrorCodes.h"
#include "Utils/Result.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"

#define MAX_FILE_NAME 64

enum OpenFlags {
	OF_OPEN_EXISTING = MASKABLE_PRAM(0),
	OF_CREATE_NEW = MASKABLE_PRAM(1),
};
enum OpenMode {
	OM_READ = 1 << MASKABLE_PRAM(0),
	OM_WRITE = 1 << MASKABLE_PRAM(1),
	OM_SERVER = 1 << MASKABLE_PRAM(2),
	OM_CLIENT = 1 << MASKABLE_PRAM(3),
};
enum class NodeType { Root, Folder, File, Pipe, Link, Device, Socket, Connection };

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

	virtual ~FSNode() {}
	virtual Result<void> open(UNUSEDARG FileDescription&) { return ResultError(ERROR_INVALID_OPERATION); }
	virtual Result<void> close(UNUSEDARG FileDescription&) { return ResultError(ERROR_INVALID_OPERATION); }
	virtual bool can_read(UNUSEDARG FileDescription&) { return false; }
	virtual bool can_write(UNUSEDARG FileDescription&) { return false; }
	virtual bool can_accept() { return false; }
	virtual bool is_connected() { return false; }
	virtual Result<void> remove() { return ResultError(ERROR_INVALID_OPERATION); }
	virtual Result<void> link(UNUSEDARG FSNode& node) { return ResultError(ERROR_INVALID_OPERATION); }
	virtual Result<void> unlink(UNUSEDARG FSNode& node) { return ResultError(ERROR_INVALID_OPERATION); }
	virtual Result<FSNode&> connect() { return ResultError(ERROR_INVALID_OPERATION); }
	virtual Result<FSNode&> accept() { return ResultError(ERROR_INVALID_OPERATION); }
	virtual Result<size_t> read(UNUSEDARG FileDescription&, UNUSEDARG void* buff, UNUSEDARG size_t offset,
	                            UNUSEDARG size_t size)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}
	virtual Result<size_t> write(UNUSEDARG FileDescription&, UNUSEDARG const void* buff, UNUSEDARG size_t offset,
	                             UNUSEDARG size_t size)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}
	virtual Result<FSNode&> dir_lookup(UNUSEDARG const StringView& file_name)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}
	virtual Result<FSNode&> create(UNUSEDARG const StringView& name, UNUSEDARG OpenMode mode, UNUSEDARG OpenFlags flags)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}
	virtual Result<void> mkdir(UNUSEDARG const StringView& name, UNUSEDARG int flags, UNUSEDARG int access)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}
	friend class VFS;
};
