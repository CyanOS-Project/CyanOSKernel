#include "PipeFS.h"
#include "Tasking/ScopedLock.h"
#include "Utils/ErrorCodes.h"
#include "Utils/PathParser.h"

UniquePointer<FSNode> PipeFS::alloc()
{
	return UniquePointer<FSNode>(new PipeFS());
}

PipeFS::PipeFS() :
    FSNode("Pipes", 0, 0, NodeType::Root, 0), //
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
