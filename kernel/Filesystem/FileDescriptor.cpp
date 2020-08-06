#include "FileDescriptor.h"
#include "Filesystem/pipes/Pipe.h"
#include "Filesystem/ustar/INode.h"
#include "utils/ErrorCodes.h"

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
	return m_node.read(buff, m_current_position, size);
}

Result<void> FileDescriptor::write(void* buff, size_t size)
{
	size_t offset = m_current_position + size;
	if (offset > m_node.m_size) {
		return ResultError(ERROR_EOF);
	}
	return m_node.read(buff, offset, size);
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
			if (((offset > 0) && (size_t(offset) > (m_node.m_size - m_current_position))) ||
			    ((offset < 0) && (size_t(-offset) > m_current_position)))
				return ResultError(ERROR_EOF);

			m_current_position = offset + m_current_position;
			break;
		}
		case SeekOrigin::END: {
			// FIXME: check overflow
			if (offset > 0)
				return ResultError(ERROR_EOF);
			if (size_t(-offset) > m_node.m_size)
				return ResultError(ERROR_EOF);

			m_current_position = m_node.m_size + offset;
			break;
		}
		default:
			break;
	}
	return ResultError(ERROR_SUCCESS);
}

Result<FileInfo> FileDescriptor::fstat()
{
	return FileInfo{m_node.m_size};
}

Result<void> FileDescriptor::ioctl()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> FileDescriptor::mmap()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}