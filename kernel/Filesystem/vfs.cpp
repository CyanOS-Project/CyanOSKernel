#include "vfs.h"

/*
File Discriptor -> fs_node -> fs_device -> file system driver
File Discriptor: is unique for each process, and contains information about the file being open such as current position
fs_node: is unique for each file/folder/device..., and contains informations about this item.
fs_device: is unique for each device, and contains operations interface.


*/
/*
FSDevice::FSDevice()
{
}

FSDevice::~FSDevice()
{
}

INode* FSDevice::open(const char* path, unsigned flags, unsigned mode)
{
    PANIC("`open` not implemented!");
}

int FSDevice::read(INode* node, void* buffer, size_t count, size_t* read_count)
{
    PANIC("`read` not implemented!");
}

int FSDevice::write(INode* node, void* buffer, size_t count, size_t* written_count)
{
    PANIC("`write` not implemented!");
}

int FSDevice::can_read(INode* node)
{
    PANIC("`can_read` not implemented!");
}

int FSDevice::can_write(INode* node)
{
    PANIC("`can_write` not implemented!");
}

int FSDevice::remove(INode* node)
{
    PANIC("`remove` not implemented!");
}

INode* FSDevice::direcotry_lookup(INode* parent, const char* name)
{
    PANIC("`direcotry_lookup` not implemented!");
}

INode::INode()
{
}

INode::~INode()
{
}

FILE::FILE()
{
}

FILE::~FILE()
{
}
*/