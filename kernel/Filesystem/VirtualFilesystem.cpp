#include "VirtualFilesystem.h"

FSNode* VFS::m_root;
VFS::VFS()
{
}

VFS::~VFS()
{
}

Result<FileDescriptor&> VFS::open(const char* path, int mode, int flags)
{
	auto node = traverse_node(path);
	if (node.is_error()) {
		return ResultError(node.error());
		// TODO: if node not found create it, if CREATE flag is set
	}
	FileDescriptor* fd = new FileDescriptor(node.value());
	return *fd;
}

Result<void> VFS::mount()
{
	return ResultError(ERROR_INVALID_PARAMETERS);
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
	size_t path_element_count = parser.path_element_count();
	if (path_element_count == 0) {
		return ResultError(ERROR_NODE_DOES_NOT_EXIST);
	}
	return traverse_node_deep(parser, path_element_count - 1);
}

Result<FSNode&> VFS::traverse_node(const char* path)
{
	PathParser parser(path);
	size_t path_element_count = parser.path_element_count();
	if (path_element_count == 0) {
		return *m_root;
	}
	return traverse_node_deep(parser, path_element_count);
}

Result<FSNode&> VFS::traverse_node_deep(PathParser& parser, size_t depth)
{
	FSNode* current = m_root;
	char last_element[MAX_FILE_NAME];
	for (size_t i = 0; i < depth; i++) {
		parser.get_element(i, last_element, MAX_FILE_NAME);
		auto next_node = current->dir_lookup(last_element);
		if (next_node.is_error())
			return ResultError(next_node.error());
		current = &next_node.value();
	}
	return *current;
}
