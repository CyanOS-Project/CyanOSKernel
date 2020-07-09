#pragma once

#include "Lib/stdlib.h"
#include "utils/types.h"

#define MAX_FILE_NAME      100
#define TAR_ALIGNMENT      512
#define TAR_OCTAL_SIZE_LEN 12

struct TarHeader {                 /* byte offset */
	char name[MAX_FILE_NAME];      /*   0 */
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

class TarFS
{
  private:
	TarHeader* m_tar_address;
	TarHeader* file_search(const char* path);
	size_t octal_to_decimal(const char* octal);
	inline uintptr_t align_to(uintptr_t address, unsigned alignment);

  public:
	TarFS(void* tar_address);
	~TarFS();
	char* read_file(const char* path);
	size_t get_file_size(const char* path);
};