#pragma once

#include "Utils/ErrorCodes.h"
#include "Utils/Result.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"

#define MAX_FILE_NAME 64
enum class OpenFlags { CreateNew, OpenExisting };
enum class OpenMode { Read, Write, ReadWrite };
enum class NodeType { Root, Folder, File, Pipe, Link, Device };

class VirtualFileSystem;

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

	virtual Result<void> open(OpenMode mode, OpenFlags flags)
	{
		UNUSED(mode);
		UNUSED(flags);

		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> close()
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> read(void* buff, size_t offset, size_t size)
	{
		UNUSED(buff);
		UNUSED(offset);
		UNUSED(size);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<void> write(const void* buff, size_t offset, size_t size)
	{
		UNUSED(buff);
		UNUSED(offset);
		UNUSED(size);
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<bool> can_read()
	{
		return ResultError(ERROR_INVALID_OPERATION);
	}

	virtual Result<bool> can_write()
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

	friend class VFS;
};
