#pragma once

#include "Utils/ErrorCodes.h"
#include "Utils/Result.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"

#define MAX_FILE_NAME 64

enum OpenFlags {
	OF_OPEN_EXISTING = 1 << 0,
	OF_CREATE_NEW = 1 << 1,
};
enum OpenMode {
	OM_READ = 1 << 0,
	OM_WRITE = 1 << 1,
	OM_SERVER = 1 << 2,
	OM_CLIENT = 1 << 3,
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

	virtual ~FSNode()
	{
	}

	virtual Result<void> open(FileDescription&)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> close(FileDescription&)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> read(FileDescription&, void* buff, size_t offset, size_t size)
	{
		UNUSED(buff);
		UNUSED(offset);
		UNUSED(size);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> write(FileDescription&, const void* buff, size_t offset, size_t size)
	{
		UNUSED(buff);
		UNUSED(offset);
		UNUSED(size);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<bool> can_read(FileDescription&)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<bool> can_write(FileDescription&)
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> remove()
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags)
	{
		UNUSED(name);
		UNUSED(mode);
		UNUSED(flags);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> mkdir(const StringView& name, int flags, int access)
	{
		UNUSED(name);
		UNUSED(flags);
		UNUSED(access);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> link(FSNode& node)
	{
		UNUSED(node);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> unlink(FSNode& node)
	{
		UNUSED(node);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<FSNode&> dir_lookup(const StringView& file_name)
	{
		UNUSED(file_name);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<FSNode&> connect()
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<FSNode&> accept()
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}
	virtual Result<bool> can_accept()
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	friend class VFS;
};
