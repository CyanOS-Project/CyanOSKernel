#pragma once

#include "Filesystem/FSNode.h"
#include "Pipe.h"
#include "Tasking/WaitQueue.h"
#include "Utils/CircularBuffer.h"
#include "Utils/List.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"
#include "Utils/UniquePointer.h"

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
};