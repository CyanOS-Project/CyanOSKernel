#include "INode.h"

INode::INode()
{
	// TODO
}

INode::~INode()
{
	// TODO
}

Result<void> INode::read(void* buff, size_t offset, size_t size)
{
	// TODO
}

Result<void> INode::write(void* buff, size_t offset, size_t size)
{
	// TODO
}

Result<bool> INode::can_read()
{
	// TODO
}

Result<bool> INode::can_write()
{
	// TODO
}

Result<void> INode::remove()
{
	// TODO
}

Result<void> INode::create(char* name, void* info)
{
	// TODO
}

Result<void> INode::mkdir(char* name, void* info)
{
	// TODO
}

Result<void> INode::link(FSNode& node)
{
	// TODO
}

Result<void> INode::unlink(FSNode& node)
{
	// TODO
}

Result<FSNode&> INode::dir_lookup(char* file_name)
{
	// TODO
}
