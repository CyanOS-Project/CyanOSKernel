#pragma once

#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include "Utils/CircularBuffer.h"
#include "Utils/List.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"
#include "Utils/UniquePointer.h"

class Connection : public FSNode
{

  private:
	const static size_t BUFFER_SIZE = 1024;
	CircularBuffer<char> m_server_buffer, m_client_buffer;
	WaitQueue m_server_wait_queue, m_client_wait_queue;
	Spinlock m_server_lock, m_client_lock;
	bool is_accepted = false;

  public:
	explicit Connection(const StringView& name);
	~Connection();
	Result<void> open(FileDescription&) override;
	Result<void> read(FileDescription&, void* buff, size_t offset, size_t size) override;
	Result<void> write(FileDescription&, const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read(FileDescription&) override;
	Result<bool> can_write(FileDescription&) override;
	Result<void> close(FileDescription&) override;

	friend class Socket;
};