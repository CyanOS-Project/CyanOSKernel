#pragma once

#include "CircularBuffer.h"
#include "Filesystem/FSNode.h"
#include "List.h"
#include "String.h"
#include "StringView.h"
#include "Tasking/WaitQueue.h"
#include "Types.h"
#include "UniquePointer.h"

class Pipe : public FSNode
{
	enum class Direction { Reader, Writer };

  private:
	const static size_t BUFFER_SIZE = 1024;
	List<Pipe> m_children;
	CircularBuffer<char> m_buffer;
	WaitQueue m_wait_queue;
	Spinlock m_lock;
	size_t m_readers;
	size_t m_writers;

  public:
	explicit Pipe(const StringView& name);
	~Pipe();
	Result<void> open(FileDescription&) override;
	Result<void> close(FileDescription&) override;
	Result<size_t> read(FileDescription&, void* buff, size_t offset, size_t size) override;
	Result<size_t> write(FileDescription&, const void* buff, size_t offset, size_t size) override;
	bool can_read(FileDescription&) override;
	bool can_write(FileDescription&) override;
	Result<void> remove() override;
};