#include "INode.h"
#include "Lib/stdlib.h"
#include "utils/ErrorCodes.h"
#include "utils/stl.h"

INode::INode(const char* name, size_t size, char* data) : m_data(data)
{
	FSNode::m_size = size;
	memcpy(m_filename, name, strlen(name) + 1);
}

INode::~INode()
{
}

Result<void> INode::read(void* buff, size_t offset, size_t size)
{
	ASSERT((offset + size) <= m_size);
	memcpy(buff, m_data + offset, size);
	return ResultError(ERROR_SUCCESS);
}

Result<void> INode::write(void* buff, size_t offset, size_t size)
{
	UNUSED(buff);
	UNUSED(offset);
	UNUSED(size);
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<bool> INode::can_read()
{
	return true;
}

Result<bool> INode::can_write()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::remove()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> INode::create(const char* name, OpenMode mode, OpenFlags flags)
{
	UNUSED(name);
	UNUSED(mode);
	UNUSED(flags);
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::mkdir(const char* name, int flags, int access)
{
	UNUSED(name);
	UNUSED(flags);
	UNUSED(access);
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::link(FSNode& node)
{
	UNUSED(node);
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::unlink(FSNode& node)
{
	UNUSED(node);
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> INode::dir_lookup(const char* file_name)
{
	for (auto& i : m_children) {
		if (strcmp(file_name, i.m_filename) == 0) {
			return i;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}
