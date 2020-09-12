#pragma once

#include "CircularBuffer.h"
#include "Filesystem/FSNode.h"
#include "List.h"
#include "Socket.h"
#include "String.h"
#include "StringView.h"
#include "Tasking/WaitQueue.h"
#include "Types.h"
#include "UniquePointer.h"

class SocketFS : public FSNode
{

  private:
	List<Socket> m_children;
	Spinlock m_lock;

	explicit SocketFS(const StringView& name);

  public:
	static UniquePointer<FSNode> alloc(const StringView& name);

	~SocketFS();
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;
};