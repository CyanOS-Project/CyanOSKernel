#pragma once

#include "INode.h"
#include <Assert.h>
#include <Clib.h>
#include <ErrorCodes.h>
#include <PathView.h>
#include <Result.h>
#include <Types.h>
#include <UniquePointer.h>

#define MAX_TAR_FILE_NAME  100
#define TAR_ALIGNMENT      512
#define TAR_OCTAL_SIZE_LEN 12
enum USTARFileType
{
	Normal = '0',
	HardLink = '1',
	SymbolicLink = '2',
	CharacterDevice = '3',
	BlockDevice = '4',
	Directory = '5',
	NamedPipe = '6'
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
	static UniquePointer<FSNode> alloc(StringView name, void* tar_address, size_t size);
	Result<void> open(UNUSEDARG FileDescription&) override;
	Result<FSNode&> dir_lookup(StringView file_name) override;
	Result<FSNode&> dir_query(UNUSEDARG size_t index) override;
	~TarFS() override;

  private:
	TarHeader* m_tar_address;

	explicit TarFS(StringView name, void* tar_address, size_t size);
	INode& add_child_node(INode& parent, StringView name, char type, const size_t size, char* data);
	String regulate_path(const char* path);
	size_t octal_to_decimal(const char* octal);
	void parse_ustar(size_t size);
};