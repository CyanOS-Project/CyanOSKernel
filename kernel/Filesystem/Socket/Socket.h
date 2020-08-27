#pragma once

#include "Connection.h"
#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include "Utils/CircularBuffer.h"
#include "Utils/List.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"
#include "Utils/UniquePointer.h"

class Socket : public FSNode
{

  private:
	List<Connection> m_pending_connections;
	List<Connection> m_connections;
	WaitQueue m_wait_queue;
	Spinlock m_lock;

  public:
	explicit Socket(const StringView& name);
	~Socket();
	Result<void> open(FileDescription&) override;
	Result<bool> can_accept() override;
	Result<FSNode&> accept() override;
	Result<FSNode&> connect() override;
	Result<void> close(FileDescription&) override;
};