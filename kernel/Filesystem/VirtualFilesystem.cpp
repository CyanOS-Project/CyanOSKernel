#include "VirtualFilesystem.h"
#include "Arch/x86/Panic.h"
#include "Assert.h"
#include "ErrorCodes.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/Thread.h"

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
	auto node = get_node(path, mode, flags);
	if (node.error()) {
		return ResultError(node.error());
	}

	auto description = UniquePointer<FileDescription>::make_unique(node.value(), mode);

	auto open_ret = node.value().open(*description);
	if (open_ret.is_error()) {
		return ResultError(open_ret.error());
	}

	return description;
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

Result<FSNode&> VFS::get_node(const StringView& path, OpenMode mode, OpenFlags flags)
{
	if (flags == OpenFlags::OF_CREATE_NEW) {
		return create_new_node(path, mode, flags);
	} else if (flags == OpenFlags::OF_OPEN_EXISTING) {
		return open_existing_node(path);
	} else {
		return ResultError(ERROR_INVALID_PARAMETERS);
	}
}

Result<FSNode&> VFS::create_new_node(const StringView& path, OpenMode mode, OpenFlags flags)
{
	PathParser parser(path);

	auto parent_node = traverse_parent_node(parser);
	if (parent_node.error()) {
		return ResultError(parent_node.error());
	}

	return parent_node.value().create(parser.element(parser.count() - 1), mode, flags);
}

Result<FSNode&> VFS::open_existing_node(const StringView& path)
{
	PathParser parser(path);

	auto node = traverse_node(parser);
	if (node.error()) {
		return ResultError(node.error());
	}

	return node.value();
}

Result<FSNode&> VFS::traverse_parent_node(const PathParser& parser)
{
	size_t path_elements_count = parser.count();
	if (path_elements_count == 0) {
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);
	}
	return traverse_node_deep(parser, path_elements_count - 1);
}

Result<FSNode&> VFS::traverse_node(const PathParser& parser)
{
	size_t path_elements_count = parser.count();
	if (path_elements_count == 0)
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);

	return traverse_node_deep(parser, path_elements_count);
}

Result<FSNode&> VFS::traverse_node_deep(const PathParser& parser, size_t depth)
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

FSNode* VFS::get_root_node(const StringView& root_name)
{
	for (auto&& i : *fs_roots) {
		if (i->m_name == root_name) {
			return i.ptr();
		}
	}
	return nullptr;
}