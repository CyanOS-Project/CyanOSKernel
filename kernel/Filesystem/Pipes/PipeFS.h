#pragma once

#include "Filesystem/FSNode.h"
#include "Pipe.h"
#include "Tasking/WaitQueue.h"
#include <CircularBuffer.h>
#include <List.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

class PipeFS : public FSNode
{
  public:
	static UniquePointer<FSNode> alloc(StringView name);

	~PipeFS();
	Result<FSNode&> create(StringView name, OpenMode mode, OpenFlags flags) override;
	Result<FSNode&> dir_lookup(StringView file_name) override;
	Result<FSNode&> dir_query(size_t index) override;

  private:
	List<Pipe> m_children;
	Spinlock m_lock;

	explicit PipeFS(StringView name);
};