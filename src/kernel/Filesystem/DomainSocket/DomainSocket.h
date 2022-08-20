#pragma once

#include "Connection.h"
#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include <CircularBuffer.h>
#include <List.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

class DomainSocket : public FSNode
{
  public:
	explicit DomainSocket(StringView name);
	~DomainSocket();
	Result<void> open(FileDescription&) override;
	bool can_accept() override;
	Result<void> connect() override;
	Result<FSNode&> accept() override;
	Result<void> close(FileDescription&) override;

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
};