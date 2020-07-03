#include "utils/types.h"
#define FS_MAX_NAME_LEN 10

class File
{
  private:
	/* data */
  public:
	File();
	~File();
	virtual int read(void* buffer, size_t count, size_t* read_count);
	virtual int write(void* buffer, size_t count, size_t* written_count);
	virtual int open(const char* path, unsigned flags, unsigned mode);
	virtual int can_read();
	virtual int can_write();
};
