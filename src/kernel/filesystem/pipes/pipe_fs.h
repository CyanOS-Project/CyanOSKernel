#pragma once

#include "filesystem/fsnode.h"
#include "pipe.h"
#include "tasking/wait_queue.h"
#include <circular_buffer.h>
#include <list.h>
#include <string.h>
#include <string_view.h>
#include <types.h>
#include <unique_pointer.h>

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