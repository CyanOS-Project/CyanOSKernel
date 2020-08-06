#pragma once

#include "Filesystem/FSNode.h"
#include "Tasking/semaphore.h"
#include "utils/CircularBuffer.h"
#include "utils/List.h"
#include "utils/types.h"

class Pipe : public FSNode
{
  private:
	const size_t BUFFER_SIZE = 1096;
	char m_filename[MAX_FILE_NAME];
	List<Pipe> m_children;
	CircularBuffer<char> m_buffer;
	Semaphore m_reader;
	Semaphore m_writer;

  public:
	explicit Pipe(const char* name);
	Pipe(const Pipe& other) = default;
	~Pipe();
	Result<void> read(void* buff, size_t offset, size_t size);
	Result<void> write(void* buff, size_t offset, size_t size);
	Result<bool> can_read();
	Result<bool> can_write();
	Result<void> remove();
	Result<void> create(const char* name, void* info);
	Result<void> mkdir(const char* name, void* info);
	Result<void> link(FSNode& node);
	Result<void> unlink(FSNode& node);
	Result<FSNode&> dir_lookup(const char* file_name);
};