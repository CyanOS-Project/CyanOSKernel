#pragma once

#include "CircularBuffer.h"
#include "Filesystem/FSNode.h"
#include "List.h"
#include "Pipe.h"
#include "String.h"
#include "StringView.h"
#include "Tasking/WaitQueue.h"
#include "Types.h"
#include "UniquePointer.h"

class PipeFS : public FSNode
{

  private:
	List<Pipe> m_children;
	Spinlock m_lock;

	explicit PipeFS(const StringView& name);

  public:
	static UniquePointer<FSNode> alloc(const StringView& name);

	~PipeFS();
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;
	Result<StringView> dir_query(size_t index) override;
};