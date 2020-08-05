#include "INode.h"

INode::INode(const char* name, size_t size, char* data) : m_data(data), m_children(new List<INode>)
{
	FSNode::m_size = size;
	memcpy(m_filename, name, strlen(name) + 1);
}

INode::INode(INode&& other) : m_data(other.m_data), m_children(other.m_children)
{
	FSNode::m_size = other.m_size;
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

Result<void> INode::create(char* name, void* info)
{
	UNUSED(name);
	UNUSED(info);
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> INode::mkdir(char* name, void* info)
{
	UNUSED(name);
	UNUSED(info);
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

Result<FSNode&> INode::dir_lookup(char* file_name)
{
	for (auto& i : *m_children) {
		if (strcmp(file_name, i.m_filename) == 0) {
			return i;
		}
	}
	return ResultError(ERROR_NODE_DOES_NOT_EXIST);
}
