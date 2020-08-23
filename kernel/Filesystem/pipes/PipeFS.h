#pragma once

#include "Filesystem/FSNode.h"
#include "Pipe.h"
#include "Tasking/WaitQueue.h"
#include "utils/CircularBuffer.h"
#include "utils/List.h"
#include "utils/String.h"
#include "utils/StringView.h"
#include "utils/UniquePointer.h"
#include "utils/types.h"

class PipeFS : public FSNode
{

  private:
	List<Pipe> m_children;
	Spinlock m_lock;

  public:
	static UniquePointer<FSNode> alloc();

	explicit PipeFS();
	~PipeFS();
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;
};