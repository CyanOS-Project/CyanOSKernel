#include "FileDescriptor.h"

FileDescriptor::FileDescriptor(FSNode& node) : m_node(node)
{
}

FileDescriptor::~FileDescriptor()
{
	close();
}

Result<void> FileDescriptor::open()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> FileDescriptor::close()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> FileDescriptor::read(void* buff, size_t size)
{
	size_t reading_size = m_current_position + size;
	if (reading_size > m_node.m_size) {
		return ResultError(ERROR_EOF);
	}
	m_node.read(buff, m_current_position, size);
	return ResultError(ERROR_SUCCESS);
}

Result<void> FileDescriptor::write(void* buff, size_t size)
{
	size_t offset = m_current_position + size;
	if (offset > m_node.m_size) {
		return ResultError(ERROR_EOF);
	}
	m_node.read(buff, offset, size);
	return ResultError(ERROR_SUCCESS);
}

Result<void> FileDescriptor::seek(int offset, SeekOrigin origin)
{
	switch (origin) {
		case SeekOrigin::SET: {
			if (offset < 0)
				return ResultError(ERROR_EOF);
			if (size_t(offset) > m_node.m_size)
				return ResultError(ERROR_EOF);

			m_current_position = offset;
			break;
		}
		case SeekOrigin::CURRENT: {
			// FIXME: check overflow
			int new_offset = offset + m_current_position;
			if (new_offset < 0)
				return ResultError(ERROR_EOF);
			if (size_t(new_offset) > m_node.m_size)
				return ResultError(ERROR_EOF);

			m_current_position = new_offset;
			break;
		}
		case SeekOrigin::END: {
			// FIXME: check overflow
			size_t new_offset = m_node.m_size + offset;
			if (new_offset < 0)
				return ResultError(ERROR_EOF);
			if (size_t(new_offset) > m_node.m_size) {
				return ResultError(ERROR_EOF);
			}

			m_current_position = new_offset;
			break;
		}
		default:
			break;
	}
	return ResultError(ERROR_SUCCESS);
}

Result<void> FileDescriptor::fstat()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> FileDescriptor::ioctl()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> FileDescriptor::mmap()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}