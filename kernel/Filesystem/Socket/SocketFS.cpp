#include "SocketFS.h"

UniquePointer<FSNode> SocketFS::alloc(const StringView& name)
{
	return UniquePointer<FSNode>(new SocketFS(name));
}

SocketFS::SocketFS(const StringView& name) :
    FSNode{name, 0, 0, NodeType::Root, 0}, //
    m_children{},
    m_lock{}
{
}

SocketFS::~SocketFS()
{
}

Result<FSNode&> SocketFS::create(const StringView& name, OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);

	if (m_children.contains([&](Socket& socket) {
		    if (socket.m_name == name)
			    return true;
		    return false;
	    }) == true) {
		return ResultError(ERROR_FILE_ALREADY_EXISTS);
	}
	return m_children.emplace_back(name);
}

Result<FSNode&> SocketFS::dir_lookup(const StringView& file_name)
{
	for (auto& child : m_children) {
		if (child.m_name == file_name) {
			return child;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}
