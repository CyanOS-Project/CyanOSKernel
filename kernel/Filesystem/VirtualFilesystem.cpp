#include "VirtualFilesystem.h"

VFS::VFS()
{
}

VFS::~VFS()
{
}

/*Result<FileDescriptor*> VFS::open(const char* path, int mode, int flags)
{
    auto node = traverse_parent_node(path);
    if (node.is_error()) {
    }

    if (flags & 1) // FIXME: if create or return error
    {
    } else {
    }

    FileDescriptor* fd = new FileDescriptor();
    fd->open();
    return fd;
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
}*/
/*
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
*/
/*Result<FSNode&> VFS::traverse_node_deep(PathParser& parser, size_t depth)
{
    FSNode* current = new Pipe;
    char last_element[MAX_FILE_NAME];
    for (size_t i = 0; i < depth; i++) {
        parser.get_element(i, last_element, MAX_FILE_NAME);
        auto next_node = current->dir_lookup(last_element);
        if (next_node.is_error()) {
            if (next_node.error() == ERROR_NODE_DOES_NOT_EXIST)
                return ResultError(ERROR_NODE_DOES_NOT_EXIST);
            return ResultError(ERROR_UNDEFINED);
        }
        current = &next_node.value();
    }
    return *current;
}*/