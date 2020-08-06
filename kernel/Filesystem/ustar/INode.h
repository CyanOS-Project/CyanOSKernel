#pragma once

#include "Filesystem/FSNode.h"
#include "utils/List.h"
#include "utils/Result.h"
#include "utils/types.h"

class INode : public FSNode
{
  private:
	char m_filename[MAX_FILE_NAME];
	char* m_data;
	List<INode> m_children;

  public:
	explicit INode(const char* name, size_t size, char* data);
	INode(const INode& other) = default;
	INode& operator=(const INode& other) = delete;
	INode& operator=(INode&& other) = delete;
	~INode();

	Result<void> read(void* buff, size_t offset, size_t size);
	Result<void> write(void* buff, size_t offset, size_t size);
	Result<bool> can_read();
	Result<bool> can_write();
	Result<void> remove();
	Result<void> create(const char* name, void* info);
	Result<void> mkdir(const char* name, void* info);
	Result<void> link(FSNode& node);
	Result<void> unlink(FSNode& node);
	Result<FSNode&> dir_lookup(const char* file_name);

	friend class TarFS;
};