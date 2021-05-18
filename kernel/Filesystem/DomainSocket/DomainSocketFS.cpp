#include "DomainSocketFS.h"

UniquePointer<FSNode> DomainSocketFS::alloc(const StringView& name)
{
	return UniquePointer<FSNode>(new DomainSocketFS(name));
}

DomainSocketFS::DomainSocketFS(const StringView& name) :
    FSNode{name, 0, 0, NodeType::Root, 0}, //
    m_children{},
    m_lock{}
{
}

DomainSocketFS::~DomainSocketFS() {}

Result<FSNode&> DomainSocketFS::create(const StringView& name, OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);

	if (m_children.contains([&](const DomainSocket& socket) { return socket.m_name == name; })) {
		return ResultError(ERROR_FILE_ALREADY_EXISTS);
	}
	return m_children.emplace_back(name);
}

Result<FSNode&> DomainSocketFS::dir_lookup(const StringView& file_name)
{
	for (auto& child : m_children) {
		if (child.m_name == file_name) {
			return child;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}
