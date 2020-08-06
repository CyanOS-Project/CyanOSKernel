#pragma once

#include "FSNode.h"
#include "utils/List.h"
#include "utils/types.h"

class Mountpoint
{
  private:
	struct MountpointMapping {
		FSNode* old_node;
		FSNode* new_node;
	};
	static List<MountpointMapping>* m_mountpoints;

  public:
	static void setup();
	static void register_mountpoint(FSNode& old_fs_node, FSNode& new_fs_node);
	static FSNode& translate_mountpoint(FSNode& node);
};
