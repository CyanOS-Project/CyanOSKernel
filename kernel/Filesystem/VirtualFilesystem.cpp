#include "VirtualFilesystem.h"

VFS::VFS()
{
}

VFS::~VFS()
{
}

int VFS::mount()
{
}

int VFS::mount_root()
{
}

int VFS::unmount()
{
}

FileDescriptor* VFS::open(const char* path, int mode, int flags)
{
	FSNode* node = traverse_parent_node(path);
	if (flags & 1) // FIXME: if create or return error
	{
		/* code */
	} else {
	}
	FileDescriptor* fd = new FileDescriptor();
	fd->open();
	return fd;
}

int VFS::remove()
{
}

int VFS::make_directory()
{
}

int VFS::remove_directory()
{
}

int VFS::rename()
{
}

int VFS::chown()
{
}

int VFS::make_link()
{
}

int VFS::remove_link()
{
}

FSNode* VFS::traverse_parent_node(const char* path)
{
	PathParser parser(path);
	FSNode* current = m_root;
	for (size_t i = 0; i < parser.path_element_count() - 1; i++) {
		current = m_root->dir_lookup();
		if (current) {
			// FIXME: check error here
		}
	}
	return current;
}

FSNode* VFS::traverse_node(const char* path)
{
	PathParser parser(path);
	FSNode* current = m_root;
	for (size_t i = 0; i < parser.path_element_count(); i++) {
		current = m_root->dir_lookup();
		if (current) {
			// FIXME: check error here
		}
	}
	return current;
}