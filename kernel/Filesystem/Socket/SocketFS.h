#pragma once

#include "Filesystem/FSNode.h"
#include "Socket.h"
#include "Tasking/WaitQueue.h"
#include "Utils/CircularBuffer.h"
#include "Utils/List.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"
#include "Utils/UniquePointer.h"

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