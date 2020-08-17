#include "Mountpoint.h"

List<Mountpoint::MountpointMapping>* Mountpoint::m_mountpoints = nullptr;

void Mountpoint::setup()
{
	m_mountpoints = new List<MountpointMapping>;
}

void Mountpoint::register_mountpoint(FSNode& old_fs_node, FSNode& new_fs_node)
{
	m_mountpoints->push_back(MountpointMapping{&old_fs_node, &new_fs_node});
}

FSNode& Mountpoint::translate_mountpoint(FSNode& node)
{
	// FIXME: use hashmap for this.
	for (auto&& i : *m_mountpoints) {
		if (i.old_node == &node)
			return *i.new_node;
	}
	return node;
}