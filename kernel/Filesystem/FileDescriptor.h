#pragma once

#include "Filesystem/pipes/Pipe.h"
#include "Filesystem/ustar/INode.h"
#include "utils/types.h"

class FileDescriptor
{
  private:
	union Node {
		INode* inode;
		Pipe* pipe;
	};
	size_t current_position;
	int type;
	bool has_changed;

  public:
	FileDescriptor();
	~FileDescriptor();
	virtual int open();
	virtual int close();
	virtual int read();
	virtual int write();
	virtual int seek();
	virtual int fstat();
	virtual int ioctl();
	virtual int mmap();
};