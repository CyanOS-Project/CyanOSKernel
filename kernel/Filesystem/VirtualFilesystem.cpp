#include "VirtualFilesystem.h"
#include "Arch/x86/Panic.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/Thread.h"
#include "Utils/Assert.h"
#include "Utils/ErrorCodes.h"

List<UniquePointer<FSNode>>* VFS::fs_roots;
Spinlock VFS::lock;

void VFS::setup()
{
	// lock the VFS and nodes.
	fs_roots = new List<UniquePointer<FSNode>>;
	lock.init();
}

Result<UniquePointer<FileDescription>> VFS::open(const StringView& path, OpenMode mode, OpenFlags flags)
{
	auto node = open_node(path, mode, flags);
	if (node.error()) {
		return ResultError(node.error());
	}

	auto open_ret = node.value().open(mode, flags);
	if (open_ret.is_error()) {
		return ResultError(open_ret.error());
	}

	return UniquePointer<FileDescription>::make_unique(node.value(), mode);
}

Result<void> VFS::mount(UniquePointer<FSNode>&& new_fs_root)
{
	if (fs_roots->contains([&](UniquePointer<FSNode>& node) {
		    if (node->m_name == new_fs_root->m_name)
			    return true;
		    return false;
	    })) {
		return ResultError(ERROR_FS_ALREADY_EXISTS);
	}
	fs_roots->push_back(move(new_fs_root));
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

Result<FSNode&> VFS::traverse_parent_node(const StringView& path)
{
	PathParser parser(path);
	size_t path_element_count = parser.count();
	if (path_element_count == 0) {
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);
	}
	return traverse_node_deep(parser, path_element_count - 1);
}

Result<FSNode&> VFS::traverse_node(const StringView& path)
{
	PathParser parser(path);
	size_t path_element_count = parser.count();
	if (path_element_count == 0)
		ResultError(ERROR_FILE_DOES_NOT_EXIST);

	return traverse_node_deep(parser, path_element_count);
}

Result<FSNode&> VFS::traverse_node_deep(PathParser& parser, size_t depth)
{
	if (fs_roots->size() == 0)
		return ResultError(ERROR_NO_ROOT_NODE);

	FSNode* current = get_root_node(parser.element(0));
	if (!current)
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);

	for (size_t i = 1; i < depth; i++) {
		auto next_node = current->dir_lookup(parser.element(i));
		if (next_node.is_error())
			return ResultError(next_node.error());
		current = &next_node.value();
	}
	return *current;
}

Result<FSNode&> VFS::open_node(const StringView& path, OpenMode mode, OpenFlags flags)
{
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
	if (open_node->m_type == NodeType::Folder) {
		return ResultError(ERROR_INVALID_OPERATION);
	}
	return *open_node;
}

FSNode* VFS::get_root_node(const StringView& root_name)
{
	for (auto&& i : *fs_roots) {
		if (i->m_name == root_name) {
			return i.ptr();
		}
	}
	return nullptr;
}