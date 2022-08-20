#include "VirtualFilesystem.h"
#include "Arch/x86/Panic.h"
#include "Tasking/Process.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/Thread.h"
#include <Assert.h>
#include <ErrorCodes.h>

List<UniquePointer<FSNode>> VFS::fs_roots;
Spinlock VFS::lock;

Result<void> VFS::mount(UniquePointer<FSNode>&& new_fs_root)
{
	if (fs_roots.contains([&](const UniquePointer<FSNode>& node) { return node->m_name == new_fs_root->m_name; })) {
		return ResultError(ERROR_FS_ALREADY_EXISTS);
	}
	fs_roots.push_back(move(new_fs_root));
	return ResultError(ERROR_SUCCESS);
}

Result<void> VFS::unmount()
{
	return ResultError(ERROR_INVALID_PARAMETER);
}

bool VFS::check_exitsts(PathView path)
{
	auto node = get_node(path, OM_READ, OF_OPEN_EXISTING);
	return !node.is_error();
}

PathView VFS::resolve_path(PathView path)
{
	if (path.type() == PathType::Relative || path.type() == PathType::RelativeRecursive) {
		path.set_absolute_path(PathView(Thread::current->parent_process().path()).sub_path(0, -2));
	}
	return path;
}

Result<FSNode&> VFS::get_node(PathView path, OpenMode mode, OpenFlags flags)
{
	if (flags == OpenFlags::OF_CREATE_NEW) {
		return create_new_node(path, mode, flags);
	} else if (flags == OpenFlags::OF_OPEN_EXISTING) {
		return open_existing_node(path);
	} else {
		return ResultError(ERROR_INVALID_PARAMETER);
	}
}

Result<FSNode&> VFS::create_new_node(PathView path, OpenMode mode, OpenFlags flags)
{
	auto parent_node = traverse_node(path.sub_path(0, -2));
	if (parent_node.error()) {
		return ResultError(parent_node.error());
	}

	return parent_node.value().create(path[path.count() - 1], mode, flags);
}

Result<FSNode&> VFS::open_existing_node(PathView path)
{
	auto node = traverse_node(path);
	if (node.error()) {
		return ResultError(node.error());
	}

	return node.value();
}

Result<FSNode&> VFS::traverse_node(PathView path)
{
	size_t path_elements_count = path.count();
	if (path_elements_count == 0)
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);

	if (fs_roots.size() == 0)
		return ResultError(ERROR_NO_ROOT_NODE);

	FSNode* current = get_root_node(path[0]);
	if (!current)
		return ResultError(ERROR_FILE_DOES_NOT_EXIST);

	for (size_t i = 1; i < path.count(); i++) {
		auto next_node = current->dir_lookup(path[i]);
		if (next_node.is_error())
			return ResultError(next_node.error());
		current = &next_node.value();
	}
	return *current;
}

FSNode* VFS::get_root_node(StringView root_name)
{
	for (auto&& i : fs_roots) {
		if (i->m_name == root_name) {
			return i.ptr();
		}
	}
	return nullptr;
}