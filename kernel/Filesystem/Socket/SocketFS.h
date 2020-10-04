#pragma once

#include "Filesystem/FSNode.h"
#include "Socket.h"
#include "Tasking/WaitQueue.h"
#include <CircularBuffer.h>
#include <List.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

class SocketFS : public FSNode
{

  public:
	static UniquePointer<FSNode> alloc(const StringView& name);
	~SocketFS();
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

  private:
	List<Socket> m_children;
	Spinlock m_lock;

	explicit SocketFS(const StringView& name);
};