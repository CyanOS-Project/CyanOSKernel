#include "INode.h"

INode::INode(const char* name, size_t size, void* data) :
    m_size(size),
    m_data(data),
    m_children(new CircularQueue<INode>)
{
	memcpy(m_filename, name, strlen(name) + 1);
}

INode::INode(INode&& other) : m_size(other.m_size), m_data(other.m_data), m_children(other.m_children)
{
	memcpy(m_filename, other.m_filename, strlen(other.m_filename) + 1);
	other.m_children = nullptr;
}

INode::~INode()
{
	if (m_children)
		delete m_children;
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
