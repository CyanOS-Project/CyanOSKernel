#include "Arch/x86/panic.h"
#include "utils/assert.h"
#define FS_MAX_NAME_LEN 10

class FSDevice
{
  private:
	/* data */
  public:
	FSDevice();
	~FSDevice();
	virtual INode* open(const char* path, unsigned flags, unsigned mode);
	virtual int read(INode* node, void* buffer, size_t count, size_t* read_count);
	virtual int write(INode* node, void* buffer, size_t count, size_t* written_count);
	virtual int can_read(INode* node);
	virtual int can_write(INode* node);
	virtual int remove(INode* node);
	virtual INode* direcotry_lookup(INode* parent, const char* name);
};

class INode
{
  private:
	const char* name;
	int flags;
	int type;
	int permission;
	size_t size;

  public:
	INode();
	~INode();
	FSDevice* file_op;
};

class FILE
{
  private:
	INode* node;
	size_t current_position;
	bool has_changed;

  public:
	FILE();
	~FILE();
};
