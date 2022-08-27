
#pragma once

#include "winnt.h"
#include <result.h>
#include <types.h>

class PELoader
{
  public:
	static Result<uintptr_t> load(const char* file, size_t size);
	PELoader() = delete;
	~PELoader() = delete;

  private:
	static void* load_pe_sections(const char* file, const IMAGE_NT_HEADERS32* nt_header);
};
