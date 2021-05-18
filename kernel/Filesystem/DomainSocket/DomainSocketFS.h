#pragma once

#include "DomainSocket.h"
#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include <CircularBuffer.h>
#include <List.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

class DomainSocketFS : public FSNode
{

  public:
	static UniquePointer<FSNode> alloc(const StringView& name);
	~DomainSocketFS();
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

  private:
	List<DomainSocket> m_children;
	Spinlock m_lock;

	explicit DomainSocketFS(const StringView& name);
};