#include "INode.h"
#include "Tasking/ScopedLock.h"
#include <Algorithms.h>
#include <Clib.h>
#include <ErrorCodes.h>

INode::INode(const StringView& name, NodeType type, size_t size, char* data) :
    FSNode(name, 0, 0, type, size),
    m_data{data},
    m_lock{},
    m_children{}
{
}

INode::~INode() {}

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

Result<size_t> INode::read(FileDescription&, BufferMutableView dest, size_t offset)
{
	ScopedLock local_lock(m_lock);

	size_t size_to_read = min(dest.size(), m_size - offset);

	dest.fill_from(m_data, offset, size_to_read);

	return size_to_read;
}

bool INode::can_read(FileDescription&)
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

Result<FSNode&> INode::dir_query(size_t index)
{
	ScopedLock local_lock(m_lock);
	if (index >= m_children.size())
		return ResultError(ERROR_INVALID_PARAMETER);

	// FIXME: it's not efficeint way to iterate them all again;
	auto itr = m_children.begin();
	while (index--) {
		itr++;
	}
	return *itr;
}