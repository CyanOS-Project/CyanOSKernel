#include "INode.h"

INode::INode()
{
}

INode::~INode()
{
}

Result<void> INode::read(void* buff, size_t offset, size_t size)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::write(void* buff, size_t offset, size_t size)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<bool> INode::can_read()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<bool> INode::can_write()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::remove()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::create(char* name, void* info)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::mkdir(char* name, void* info)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::link(FSNode& node)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::unlink(FSNode& node)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> INode::dir_lookup(char* file_name)
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}
