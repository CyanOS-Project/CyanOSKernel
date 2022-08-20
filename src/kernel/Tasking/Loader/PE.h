
#pragma once

#include "Winnt.h"
#include <Result.h>
#include <Types.h>

class PELoader
{
  public:
	static Result<uintptr_t> load(const char* file, size_t size);
	PELoader() = delete;
	~PELoader() = delete;

  private:
	static void* load_pe_sections(const char* file, const IMAGE_NT_HEADERS32* nt_header);
};
