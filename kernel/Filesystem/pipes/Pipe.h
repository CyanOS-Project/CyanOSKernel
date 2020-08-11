#pragma once

#include "Filesystem/FSNode.h"
#include "Tasking/semaphore.h"
#include "utils/CircularBuffer.h"
#include "utils/List.h"
#include "utils/types.h"

class Pipe : public FSNode
{
	enum class Direction { Reader, Writer };

  private:
	const size_t BUFFER_SIZE = 1096;
	char m_filename[MAX_FILE_NAME];
	List<Pipe> m_children;
	CircularBuffer<char> m_buffer;
	Semaphore m_reader;
	Semaphore m_writer;
	WaitQueue m_wait_queue;
	const Direction m_direction;

  public:
	explicit Pipe(const char* name, Direction direction);
	Pipe();
	Pipe(const Pipe& other) = default;
	~Pipe();
	Result<void> read(void* buff, size_t offset, size_t size);
	Result<void> write(void* buff, size_t offset, size_t size);
	Result<bool> can_read();
	Result<bool> can_write();
	Result<void> remove();
	Result<FSNode&> create(const char* name, OpenMode mode, OpenFlags flags);
	Result<void> mkdir(const char* name, int flags, int access);
	Result<void> link(FSNode& node);
	Result<void> unlink(FSNode& node);
	Result<FSNode&> dir_lookup(const char* file_name);

	static Pipe& root_node();
};