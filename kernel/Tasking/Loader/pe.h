
#pragma once

#include "utils/Result.h"
#include "utils/types.h"
#include "winnt.h"

class PELoader
{
  private:
	static uintptr_t align_to(uintptr_t size, size_t alignment);
	static void* load_pe_sections(const char* file, const IMAGE_NT_HEADERS32* nt_header);

  public:
	static Result<uintptr_t> load(const char* file, size_t size);
	PELoader() = delete;
	~PELoader() = delete;
};
