#pragma once

#include "CircularBuffer.h"
#include "Connection.h"
#include "Filesystem/FSNode.h"
#include "List.h"
#include "String.h"
#include "StringView.h"
#include "Tasking/WaitQueue.h"
#include "Types.h"
#include "UniquePointer.h"

class Socket : public FSNode
{
  private:
	struct NewPendingConnection {
		bool is_accepted;
		Connection* connection;
		NewPendingConnection() : is_accepted{false}, connection{nullptr} {}
	};

	List<NewPendingConnection> m_pending_connections;
	List<Connection> m_connections;
	WaitQueue m_server_wait_queue;
	WaitQueue m_connections_wait_queue;
	Spinlock m_lock;

  public:
	explicit Socket(const StringView& name);
	~Socket();
	Result<void> open(FileDescription&) override;
	bool can_accept() override;
	Result<FSNode&> accept() override;
	Result<FSNode&> connect() override;
	Result<void> close(FileDescription&) override;
};