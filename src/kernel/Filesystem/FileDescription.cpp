#include "FileDescription.h"
#include "FSNode.h"
#include "FileSystemDefinitions.h"
#include "IPSocket/IPSocket.h"
#include "VirtualFilesystem.h"
#include <Clib.h>
#include <ErrorCodes.h>

Result<UniquePointer<FileDescription>> FileDescription::open(PathView path, OpenMode mode, OpenFlags flags)
{
	// FIXME: Fail if there is someone already has a handle with writing permission.

	auto node = VFS::get_node(path, mode, flags);
	if (node.error())
		return ResultError{node.error()};

	auto description = UniquePointer<FileDescription>::make_unique(node.value(), mode);

	auto open_ret = node.value().open(*description);
	if (open_ret.is_error())
		return ResultError{open_ret.error()};

	return description;
}

Result<UniquePointer<FileDescription>> FileDescription::socket(SocketAddress addr, SocketProtocol protocol,
                                                               size_t flags)
{
	auto node = create_socket(addr, protocol, flags);
	if (node.error())
		return ResultError{node.error()};

	// FIXME: there should be OpenMode paramter here.
	auto description = UniquePointer<FileDescription>::make_unique(node.value(), OpenMode::OM_SERVER);

	auto open_ret = node.value().open(*description);
	if (open_ret.is_error())
		return ResultError{open_ret.error()};

	return description;
}

FileDescription::FileDescription(FSNode& node, OpenMode mode) : m_type{node.m_type}, m_mode{mode}, m_node(node) {}

FileDescription::~FileDescription()
{
	// FIXME: delete socket too here.
	m_node.close(*this);
}

Result<size_t> FileDescription::read(BufferMutableView buffer)
{
	size_t reading_size = m_current_position + buffer.size();
	if (reading_size > m_node.m_size && m_node.m_size != 0) {
		return ResultError(ERROR_EOF);
	}
	return m_node.read(*this, buffer, m_current_position);
}

Result<size_t> FileDescription::write(BufferView buffer)
{
	size_t offset = m_current_position + buffer.size();
	if (offset > m_node.m_size && m_node.m_size != 0) {
		return ResultError(ERROR_EOF);
	}
	return m_node.write(*this, buffer, offset);
}

Result<void> FileDescription::connect()
{
	auto connection_desc = m_node.connect();
	if (connection_desc.is_error())
		return ResultError(connection_desc.error());

	return {};
	// return UniquePointer<FileDescription>::make_unique(connection_desc.value(), OpenMode::OM_CLIENT);
}

Result<UniquePointer<FileDescription>> FileDescription::accept()
{
	auto connection_desc = m_node.accept();
	if (connection_desc.is_error())
		return ResultError(connection_desc.error());

	return UniquePointer<FileDescription>::make_unique(connection_desc.value(), OpenMode::OM_SERVER);
}

Result<void> FileDescription::seek(int offset, SeekOrigin origin)
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

Result<void> FileDescription::dir_query(FileInfo& info)
{
	auto node = m_node.dir_query(m_current_position);
	if (node.is_error())
		return ResultError(node.error());

	memcpy(info.file_name, node.value().m_name.c_str(), node.value().m_name.length());
	info.file_name[node.value().m_name.length()] = 0;
	info.type = node.value().m_type;
	m_current_position++;

	return ResultError(ERROR_SUCCESS);
}

// FIXME: return a value directly!
Result<void> FileDescription::file_query(FileInfo& info)
{
	memcpy(info.file_name, m_node.m_name.c_str(), m_node.m_name.length());
	info.file_name[m_node.m_name.length()] = 0;
	info.size = m_node.m_size;
	info.type = m_node.m_type;
	return ResultError(ERROR_SUCCESS);
}

Result<void> FileDescription::ioctl()
{
	return ResultError(ERROR_INVALID_PARAMETER);
}

Result<void> FileDescription::mmap()
{
	return ResultError(ERROR_INVALID_PARAMETER);
}