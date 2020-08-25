#include "PipeFS.h"
#include "Tasking/ScopedLock.h"
#include "Utils/ErrorCodes.h"
#include "Utils/PathParser.h"

UniquePointer<FSNode> PipeFS::alloc(const StringView& name)
{
	return UniquePointer<FSNode>(new PipeFS(name));
}

PipeFS::PipeFS(const StringView& name) :
    FSNode(name, 0, 0, NodeType::Root, 0), //
    m_children{},
    m_lock{}
{
	// FIXME: multiple writers, one reader.
	m_lock.init();
}

PipeFS::~PipeFS()
{
}

Result<FSNode&> PipeFS::create(const StringView& name, OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);
	return m_children.emplace_back(name);
}

Result<FSNode&> PipeFS::dir_lookup(const StringView& file_name)
{
	for (auto& i : m_children) {
		if (i.m_name == file_name) {
			return i;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}
