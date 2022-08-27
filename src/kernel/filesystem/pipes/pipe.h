#pragma once

#include "filesystem/fsnode.h"
#include "tasking/wait_queue.h"
#include <circular_buffer.h>
#include <list.h>
#include <string.h>
#include <string_view.h>
#include <types.h>
#include <unique_pointer.h>

class Pipe : public FSNode
{
  public:
	explicit Pipe(StringView name);
	~Pipe();
	Result<void> open(FileDescription&) override;
	Result<void> close(FileDescription&) override;
	Result<size_t> read(FileDescription&, BufferMutableView dest, size_t offset) override;
	Result<size_t> write(FileDescription&, BufferView src, size_t offset) override;
	bool can_read(FileDescription&) override;
	bool can_write(FileDescription&) override;
	Result<void> remove() override;

  private:
	enum class Direction
	{
		Reader,
		Writer
	};

	const static size_t BUFFER_SIZE = 1024;
	List<Pipe> m_children;
	CircularBuffer<char> m_buffer;
	WaitQueue m_wait_queue;
	Spinlock m_lock;
	size_t m_readers;
	size_t m_writers;
};