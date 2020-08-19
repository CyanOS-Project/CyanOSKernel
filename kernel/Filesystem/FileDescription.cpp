#include "FileDescription.h"
#include "Filesystem/pipes/Pipe.h"
#include "Filesystem/ustar/INode.h"
#include "utils/ErrorCodes.h"

FileDescription::FileDescription(FSNode& node) : m_node(node)
{
}

FileDescription::~FileDescription()
{
	if (m_is_open) {
		close();
	}
}

Result<void> FileDescription::close()
{
	ASSERT(m_is_open);
	m_is_open = false;
	return m_node.close();
}

Result<void> FileDescription::read(void* buff, size_t size)
{
	ASSERT(m_is_open);

	size_t reading_size = m_current_position + size;
	if (reading_size > m_node.m_size) {
		return ResultError(ERROR_EOF);
	}
	return m_node.read(buff, m_current_position, size);
}

Result<void> FileDescription::write(const void* buff, size_t size)
{
	ASSERT(m_is_open);

	size_t offset = m_current_position + size;
	if (offset > m_node.m_size) {
		return ResultError(ERROR_EOF);
	}
	return m_node.write(buff, offset, size);
}

Result<void> FileDescription::seek(int offset, SeekOrigin origin)
{
	ASSERT(m_is_open);

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
			if (((offset > 0) && (size_t(offset) > (m_node.m_size - m_current_position))) ||
			    ((offset < 0) && (size_t(-offset) > m_current_position)))
				return ResultError(ERROR_EOF);

			m_current_position = offset + m_current_position;
			break;
		}
		case SeekOrigin::END: {
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

Result<FileInfo> FileDescription::fstat()
{
	ASSERT(m_is_open);

	return FileInfo{m_node.m_size};
}

Result<void> FileDescription::ioctl()
{
	ASSERT(m_is_open);

	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> FileDescription::mmap()
{
	ASSERT(m_is_open);

	return ResultError(ERROR_INVALID_PARAMETERS);
}