#pragma once

#include "Filesystem/FSNode.h"
#include "utils/ErrorCodes.h"
#include "utils/Result.h"
#include "utils/types.h"

class INode : public FSNode
{
  private:
  public:
	INode();
	~INode();
	Result<void> read(void* buff, size_t offset, size_t size);
	Result<void> write(void* buff, size_t offset, size_t size);
	Result<bool> can_read();
	Result<bool> can_write();
	Result<void> remove();
	Result<void> create(char* name, void* info);
	Result<void> mkdir(char* name, void* info);
	Result<void> link(FSNode& node);
	Result<void> unlink(FSNode& node);
	Result<FSNode&> dir_lookup(char* file_name);
};