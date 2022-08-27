#pragma once

#include "filesystem/fsnode.h"
#include "tasking/wait_queue.h"
#include <circular_buffer.h>
#include <list.h>
#include <string.h>
#include <string_view.h>
#include <types.h>
#include <unique_pointer.h>

class Connection : public FSNode
{
  public:
	explicit Connection(StringView name);
	~Connection();
	Result<void> open(FileDescription&) override;
	Result<size_t> read(FileDescription&, BufferMutableView dest, size_t offset) override;
	Result<size_t> write(FileDescription&, BufferView src, size_t offset) override;
	bool can_read(FileDescription&) override;
	bool can_write(FileDescription&) override;
	Result<void> close(FileDescription&) override;

  private:
	const static size_t BUFFER_SIZE = 1024;
	CircularBuffer<char> m_server_buffer, m_client_buffer;
	WaitQueue m_server_wait_queue, m_client_wait_queue;
	Spinlock m_server_lock, m_client_lock;

	size_t read_client_buffer(FileDescription&, BufferMutableView dest);
	size_t read_server_buffer(FileDescription&, BufferMutableView dest);
	size_t write_client_buffer(FileDescription&, BufferView src);
	size_t write_server_buffer(FileDescription&, BufferView src);

	friend class Socket;
};