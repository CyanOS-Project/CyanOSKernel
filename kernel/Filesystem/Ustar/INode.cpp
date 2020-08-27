#include "INode.h"
#include "Lib/Stdlib.h"
#include "Tasking/ScopedLock.h"
#include "Utils/ErrorCodes.h"
#include "Utils/Stl.h"

INode::INode(const StringView& name, NodeType type, size_t size, char* data) :
    FSNode(name, 0, 0, type, size),
    m_data{data},
    m_lock{},
    m_children{}
{
	m_lock.init();
}

INode::~INode()
{
}

Result<void> INode::open(FileDescription&)
{
	ScopedLock local_lock(m_lock);
	return ResultError(ERROR_SUCCESS);
}

Result<void> INode::close(FileDescription&)
{
	ScopedLock local_lock(m_lock);
	return ResultError(ERROR_SUCCESS);
}

Result<void> INode::read(FileDescription&, void* buff, size_t offset, size_t size)
{
	ScopedLock local_lock(m_lock);
	ASSERT((offset + size) <= m_size);
	memcpy(buff, m_data + offset, size);
	return ResultError(ERROR_SUCCESS);
}

Result<bool> INode::can_read(FileDescription&)
{
	return true;
}

Result<FSNode&> INode::dir_lookup(const StringView& file_name)
{
	ScopedLock local_lock(m_lock);
	for (auto& i : m_children) {
		if (i.m_name == file_name) {
			return i;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}
