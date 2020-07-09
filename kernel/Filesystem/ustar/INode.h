#pragma once

#include "Filesystem/FSNode.h"
#include "utils/types.h"

class INode : public FSNode
{
  private:
  public:
	INode();
	~INode();
	virtual int read();
	virtual int write();
	virtual int can_read();
	virtual int can_write();
	virtual int lookup_directory();
	virtual int remove_directory();
	virtual int make_direcotry();
	virtual int make_file();
	virtual int remove_file();
};