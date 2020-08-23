#pragma once

#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include "utils/CircularBuffer.h"
#include "utils/List.h"
#include "utils/String.h"
#include "utils/StringView.h"
#include "utils/UniquePointer.h"
#include "utils/types.h"

class Pipe : public FSNode
{
	enum class Direction { Reader, Writer };

  private:
	const static size_t BUFFER_SIZE = 1024;
	List<Pipe> m_children;
	CircularBuffer<char> m_buffer;
	WaitQueue m_wait_queue;
	Spinlock m_lock;

  public:
	explicit Pipe(const StringView& name);
	~Pipe();
	Result<void> open(OpenMode mode, OpenFlags flags) override;
	Result<void> close() override;
	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<void> write(const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<bool> can_write() override;
	Result<void> remove() override;
};