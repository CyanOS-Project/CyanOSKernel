#pragma once

#include "connection.h"
#include "filesystem/fsnode.h"
#include "tasking/wait_queue.h"
#include <circular_buffer.h>
#include <list.h>
#include <string.h>
#include <string_view.h>
#include <types.h>
#include <unique_pointer.h>

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