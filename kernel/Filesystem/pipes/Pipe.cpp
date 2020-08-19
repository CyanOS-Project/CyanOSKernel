#include "Pipe.h"
#include "Tasking/ScopedLock.h"
#include "utils/ErrorCodes.h"
#include "utils/PathParser.h"

Pipe& Pipe::root_node()
{
	return *new Pipe(".", FSNode::NodeType::Root);
}

Pipe::Pipe(const StringView& name, FSNode::NodeType type) :
    FSNode(0, 0, type, BUFFER_SIZE),
    m_filename{name},
    m_children{},
    m_buffer{BUFFER_SIZE},
    m_wait_queue{},
    m_lock{}
{
	// FIXME: multiple writers, one reader.
}

Pipe::~Pipe()
{
}

Result<void> Pipe::open(OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);

	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::close()
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::read(void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);
	if (m_buffer.size() < size) {
		local_lock.release();
		Thread::current->wait_on(m_wait_queue);
		local_lock.acquire();
	}

	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size; i++) {
		_buf[i] = m_buffer.dequeue();
	}

	m_wait_queue.wake_up();
	return ResultError(ERROR_SUCCESS);
}

Result<void> Pipe::write(const void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);
	if (m_buffer.available_size() < size) {
		local_lock.release();
		Thread::current->wait_on(m_wait_queue);
		local_lock.acquire();
	}

	const char* _buf = static_cast<const char*>(buff);
	for (size_t i = 0; i < size; i++) {
		m_buffer.queue(_buf[i]);
	}

	m_wait_queue.wake_up();
	return ResultError(ERROR_SUCCESS);
}

Result<bool> Pipe::can_read()
{
	return m_buffer.is_empty();
}

Result<bool> Pipe::can_write()
{
	return m_buffer.is_full();
}

Result<void> Pipe::remove()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> Pipe::create(const StringView& name, OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);
	return m_children.emplace_back(name, FSNode::NodeType::File);
}

Result<void> Pipe::mkdir(const StringView& name, int flags, int access)
{
	UNUSED(name);
	UNUSED(flags);
	UNUSED(access);
	PANIC("mkdir not implemented");
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::link(FSNode& node)
{
	UNUSED(node);
	PANIC("link not implemented");
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> Pipe::unlink(FSNode& node)
{
	UNUSED(node);
	PANIC("unlink not implemented");
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> Pipe::dir_lookup(const StringView& file_name)
{
	for (auto& i : m_children) {
		if (i.m_filename == file_name) {
			return i;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}
