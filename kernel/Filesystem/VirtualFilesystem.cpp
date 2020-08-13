#include "VirtualFilesystem.h"
#include "Arch/x86/panic.h"
#include "Mountpoint.h"
#include "Tasking/Thread.h"
#include "pipes/Pipe.h"
#include "ustar/INode.h"
#include "utils/ErrorCodes.h"
#include "utils/assert.h"

FSNode* VFS::m_root;
List<FileDescription>* VFS::m_file_description;

void VFS::setup()
{
	m_file_description = new List<FileDescription>;
	Mountpoint::setup();
}

Result<FileDescription&> VFS::open(const char* path, OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);
	auto node = traverse_node(path);
	FSNode* open_node = nullptr;

	if ((node.error() != ERROR_FILE_DOES_NOT_EXIST) && (flags == OpenFlags::CreateNew)) {
		return ResultError(ERROR_FILE_ALREADY_EXISTS);
	} else if ((node.error() == ERROR_FILE_DOES_NOT_EXIST) && (flags == OpenFlags::CreateNew)) {
		// FIXME: we already went though parent! any optimization ?
		auto parent_node = traverse_parent_node(path);
		ASSERT(!parent_node.is_error());
		PathParser parser(path);
		auto new_node = parent_node.value().create(parser.element(parser.count() - 1), mode, flags);
		if (new_node.is_error()) {
			return ResultError(new_node.error());
		}
		open_node = &new_node.value();
	} else if (node.is_error()) {
		return ResultError(node.error());
	} else {
		open_node = &node.value();
	}

	FileDescription& fd = m_file_description->emplace_back(*open_node);
	Thread::current->parent_process().file_descriptors.push_back(&fd);
	return fd;
}

Result<void> VFS::mount(const char* path, FSNode& m_root_node)
{
	auto node_result = traverse_node(path);
	if (node_result.is_error())
		return ResultError(node_result.error());

	Mountpoint::register_mountpoint(node_result.value(), m_root_node);
	return ResultError(ERROR_SUCCESS);
}

Result<void> VFS::mount_root(FSNode& node)
{
	if (m_root)
		return ResultError(ERROR_MOUNTPOINT_EXISTS);
	m_root = &node;
	return ResultError(ERROR_SUCCESS);
}

Result<void> VFS::unmount()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> VFS::remove()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> VFS::make_directory()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> VFS::remove_directory()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> VFS::rename()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> VFS::chown()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> VFS::make_link()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<void> VFS::remove_link()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
}

Result<FSNode&> VFS::traverse_parent_node(const char* path)
{
	PathParser parser(path);
	size_t path_element_count = parser.count();
	if (path_element_count == 0) {
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);
	}
	return traverse_node_deep(parser, path_element_count - 1);
}

Result<FSNode&> VFS::traverse_node(const char* path)
{
	PathParser parser(path);
	size_t path_element_count = parser.count();
	if (path_element_count == 0) {
		return *m_root;
	}
	return traverse_node_deep(parser, path_element_count);
}

Result<FSNode&> VFS::traverse_node_deep(PathParser& parser, size_t depth)
{
	if (!m_root)
		return ResultError(ERROR_NO_ROOT_NODE);

	FSNode* current = m_root;
	for (size_t i = 0; i < depth; i++) {
		auto next_node = current->dir_lookup(parser.element(i));
		if (next_node.is_error())
			return ResultError(next_node.error());
		current = &Mountpoint::translate_mountpoint(next_node.value());
	}
	return *current;
}
