#include "ELF.h"
#include "VirtualMemory/Memory.h"
#include <ErrorCodes.h>

Result<ExecutableInformation> ELFLoader::load(const char* file, size_t size)
{
	ELFParser elf(file, size);
	if (!elf.is_valid()) {
		return ResultError(ERROR_INVALID_EXECUTABLE);
	}

	if (!load_segments(file, elf)) {
		// FIXME: memory leak if not all sections are loaded!
		return ResultError(ERROR_LOADING_EXECUTABLE);
	}
	auto& init_array_section = elf.section_header_by_type(SHT_INIT_ARRAY);
	return ExecutableInformation{.entry_point = elf.entry_point(),
	                             .constructors_array = reinterpret_cast<uintptr_t*>(init_array_section.sh_addr),
	                             .constructors_array_count = init_array_section.sh_size / sizeof(uintptr_t)};
}

bool ELFLoader::load_segments(const char* file, const ELFParser& elf)
{
	for (size_t i = 0; i < elf.programs_number(); i++) {
		if (elf.program_header_by_index(i).p_memsz == 0) {
			continue;
		}

		void* addr = valloc(reinterpret_cast<void*>(elf.program_header_by_index(i).p_vaddr),
		                    elf.program_header_by_index(i).p_memsz, PAGE_USER | PAGE_READWRITE);
		if (!addr) {
			return false;
		}
		memcpy(addr, file + elf.program_header_by_index(i).p_offset, elf.program_header_by_index(i).p_filesz);
	}
	return true;
}