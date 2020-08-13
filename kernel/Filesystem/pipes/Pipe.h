#pragma once

#include "Filesystem/FSNode.h"
#include "Tasking/semaphore.h"
#include "utils/CircularBuffer.h"
#include "utils/List.h"
#include "utils/String.h"
#include "utils/StringView.h"
#include "utils/types.h"

class Pipe : public FSNode
{
	enum class Direction { Reader, Writer };

  private:
	const static size_t BUFFER_SIZE = 1096;
	String m_filename;
	List<Pipe> m_children;
	CircularBuffer<char> m_buffer;
	WaitQueue m_wait_queue;
	// const Direction m_direction;

  public:
	explicit Pipe(const StringView& name);
	Pipe();
	Pipe(const Pipe& other) = default;
	~Pipe();
	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<void> write(const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<bool> can_write() override;
	Result<void> remove() override;
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<void> mkdir(const StringView& name, int flags, int access) override;
	Result<void> link(FSNode& node) override;
	Result<void> unlink(FSNode& node) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

	static Pipe& root_node();
};