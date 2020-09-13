#pragma once

#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include <CircularBuffer.h>
#include <List.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>
#include <UniquePointer.h>

class Connection : public FSNode
{

  private:
	const static size_t BUFFER_SIZE = 1024;
	CircularBuffer<char> m_server_buffer, m_client_buffer;
	WaitQueue m_server_wait_queue, m_client_wait_queue;
	Spinlock m_server_lock, m_client_lock;

	size_t read_client_buffer(FileDescription&, void* buff, size_t size);
	size_t read_server_buffer(FileDescription&, void* buff, size_t size);
	size_t write_client_buffer(FileDescription&, const void* buff, size_t size);
	size_t write_server_buffer(FileDescription&, const void* buff, size_t size);

  public:
	explicit Connection(const StringView& name);
	~Connection();
	Result<void> open(FileDescription&) override;
	Result<size_t> read(FileDescription&, void* buff, size_t offset, size_t size) override;
	Result<size_t> write(FileDescription&, const void* buff, size_t offset, size_t size) override;
	bool can_read(FileDescription&) override;
	bool can_write(FileDescription&) override;
	Result<void> close(FileDescription&) override;

	friend class Socket;
};