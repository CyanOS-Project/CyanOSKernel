#pragma once

#include "INode.h"
#include "Lib/stdlib.h"
#include "utils/ErrorCodes.h"
#include "utils/PathParser.h"
#include "utils/Result.h"
#include "utils/UniquePointer.h"
#include "utils/assert.h"
#include "utils/types.h"

#define MAX_TAR_FILE_NAME  100
#define TAR_ALIGNMENT      512
#define TAR_OCTAL_SIZE_LEN 12
enum USTARFileType {
	NORMAL = '0',
	HARD_LINK = '1',
	SYMBOLIC_LINK = '2',
	CHARACTER_DEVICE = '3',
	BLOCK_DEVICE = '4',
	DIRECTORY = '5',
	NAMED_PIPE = '6'
};

struct TarHeader {                 /* byte offset */
	char name[MAX_TAR_FILE_NAME];  /*   0 */
	char mode[8];                  /* 100 */
	char uid[8];                   /* 108 */
	char gid[8];                   /* 116 */
	char size[TAR_OCTAL_SIZE_LEN]; /* 124 */
	char mtime[12];                /* 136 */
	char chksum[8];                /* 148 */
	char typeflag;                 /* 156 */
	char linkname[100];            /* 157 */
	char magic[6];                 /* 257 */
	char version[2];               /* 263 */
	char uname[32];                /* 265 */
	char gname[32];                /* 297 */
	char devmajor[8];              /* 329 */
	char devminor[8];              /* 337 */
	char prefix[167];              /* 345 */
	                               /* 500 */
};

class TarFS : public INode
{
  private:
	TarHeader* m_tar_address;

	TarFS(void* tar_address, size_t size);
	INode& add_child_node(INode& parent, const StringView& name, char type, const size_t size, char* data);
	String regulate_path(const char* path);
	size_t octal_to_decimal(const char* octal);
	inline uintptr_t align_to(uintptr_t address, unsigned alignment);
	void parse_ustar(size_t size);

  public:
	static UniquePointer<FSNode> alloc(void* tar_address, size_t size);

	Result<void> open(OpenMode mode, OpenFlags flags) override;
	Result<void> close() override;
	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<void> write(const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<bool> can_write() override;
	Result<void> remove() override;
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<void> mkdir(const StringView& name, int flags, int access) override;
	Result<void> link(FSNode& node) override;
	Result<void> unlink(FSNode& node) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;
	~TarFS() override;
};