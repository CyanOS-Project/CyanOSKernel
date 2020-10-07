#pragma once

#include "INode.h"
#include <Assert.h>
#include <Clib.h>
#include <ErrorCodes.h>
#include <PathParser2.h>
#include <Result.h>
#include <Types.h>
#include <UniquePointer.h>

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
  public:
	static UniquePointer<FSNode> alloc(const StringView& name, void* tar_address, size_t size);
	Result<void> open(UNUSEDARG FileDescription&) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;
	Result<StringView> dir_query(UNUSEDARG size_t index) override;
	~TarFS() override;

  private:
	TarHeader* m_tar_address;

	explicit TarFS(const StringView& name, void* tar_address, size_t size);
	INode& add_child_node(INode& parent, const StringView& name, char type, const size_t size, char* data);
	String regulate_path(const char* path);
	size_t octal_to_decimal(const char* octal);
	void parse_ustar(size_t size);
};