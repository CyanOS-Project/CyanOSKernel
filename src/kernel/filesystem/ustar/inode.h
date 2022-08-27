#pragma once

#include "filesystem/fsnode.h"
#include "tasking/spin_lock.h"
#include <list.h>
#include <result.h>
#include <string.h>
#include <types.h>

class INode : public FSNode
{
  private:
	char* m_data;
	Spinlock m_lock;
	List<INode> m_children;

  public:
	explicit INode(StringView name, NodeType type, size_t size, char* data);
	INode& operator=(const INode& other) = delete;
	INode& operator=(INode&& other) = delete;
	~INode();
	Result<void> open(FileDescription&) override;
	Result<void> close(FileDescription&) override;
	Result<size_t> read(FileDescription&, BufferMutableView dest, size_t offset) override;
	bool can_read(FileDescription&) override;
	Result<FSNode&> dir_lookup(StringView file_name) override;
	Result<FSNode&> dir_query(size_t index) override;

	friend class TarFS;
};