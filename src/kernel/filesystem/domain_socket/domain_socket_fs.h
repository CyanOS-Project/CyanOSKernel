#pragma once

#include "domain_socket.h"
#include "filesystem/fsnode.h"
#include "tasking/wait_queue.h"
#include <circular_buffer.h>
#include <list.h>
#include <string.h>
#include <string_view.h>
#include <types.h>
#include <unique_pointer.h>

class DomainSocketFS : public FSNode
{

  public:
	static UniquePointer<FSNode> alloc(StringView name);
	~DomainSocketFS();
	Result<FSNode&> create(StringView name, OpenMode mode, OpenFlags flags) override;
	Result<FSNode&> dir_lookup(StringView file_name) override;

  private:
	List<DomainSocket> m_children;
	Spinlock m_lock;

	explicit DomainSocketFS(StringView name);
};