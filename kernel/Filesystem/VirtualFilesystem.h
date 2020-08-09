#pragma once

#include "FSNode.h"
#include "FileDescription.h"
#include "utils/List.h"
#include "utils/PathParser.h"
#include "utils/Result.h"
#include "utils/types.h"

class VFS
{
  private:
	static FSNode* m_root;
	static List<FileDescription>* m_file_description;

	static Result<FSNode&> traverse_parent_node(const char* path);
	static Result<FSNode&> traverse_node(const char* path);
	static Result<FSNode&> traverse_node_deep(PathParser& parser, size_t depth);

  public:
	static void setup();
	static Result<FileDescription&> open(const char* path, int mode, int flags);
	static Result<void> mount(const char* path, FSNode& m_root_node);
	static Result<void> mount_root(FSNode& node);
	static Result<void> unmount();
	static Result<void> remove();
	static Result<void> make_directory();
	static Result<void> remove_directory();
	static Result<void> rename();
	static Result<void> chown();
	static Result<void> make_link();
	static Result<void> remove_link();
};
