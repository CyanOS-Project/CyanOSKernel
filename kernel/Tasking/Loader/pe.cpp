#include "pe.h"

Result<uintptr_t> PELoader::load(const char* file, size_t size)
{
	const IMAGE_DOS_HEADER* dos_header = reinterpret_cast<const IMAGE_DOS_HEADER*>(file);
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
		return ResultError(ERROR_INVALID_EXECUTABLE);
	if (!dos_header->e_lfanew)
		return ResultError(ERROR_INVALID_EXECUTABLE);

	const IMAGE_NT_HEADERS32* nt_header = reinterpret_cast<const IMAGE_NT_HEADERS32*>(file + dos_header->e_lfanew);
	if (nt_header->Signature != IMAGE_NT_SIGNATURE)
		return ResultError(ERROR_INVALID_EXECUTABLE);

	const IMAGE_SECTION_HEADER* section_header = reinterpret_cast<const IMAGE_SECTION_HEADER*>(nt_header + 1);
	for (size_t i = 0; i < nt_header->FileHeader.NumberOfSections; i++) {
		if (!section_header[i].VirtualAddress || !section_header[i].PointerToRawData ||
		    !section_header[i].Misc.VirtualSize)
			return ResultError(ERROR_INVALID_EXECUTABLE);
	}

	void* start_of_executable = load_pe_sections(file, nt_header);
	if (!start_of_executable)
		return ResultError(ERROR_LOADING_EXECUTABLE);

	return uintptr_t(start_of_executable) + nt_header->OptionalHeader.AddressOfEntryPoint;
}

void* PELoader::load_pe_sections(const char* file, const IMAGE_NT_HEADERS32* nt_header)
{
	const IMAGE_SECTION_HEADER* section_header = reinterpret_cast<const IMAGE_SECTION_HEADER*>(nt_header + 1);
	const uint32_t section_alignment = nt_header->OptionalHeader.SectionAlignment;
	// Load header
	void* start_of_executable = Memory::alloc(nt_header->OptionalHeader.SectionAlignment, MEMORY_TYPE::WRITABLE);
	memset(start_of_executable, 0, section_alignment);
	memcpy(start_of_executable, file, sizeof(IMAGE_DOS_HEADER));
	// load sections
	void* current_section_address = reinterpret_cast<void*>(uintptr_t(start_of_executable) + section_alignment);
	for (size_t i = 0; i < nt_header->FileHeader.NumberOfSections; i++) {

		void* current_section_address = Memory::alloc(current_section_address,                     //
		                                              align_to(section_header[i].Misc.VirtualSize, //
		                                                       section_alignment),                 //
		                                              MEMORY_TYPE::WRITABLE);                      //
		if (!current_section_address)
			PANIC("Loading Executable Image invalid memory region");
		memcpy(current_section_address, file + section_header[i].PointerToRawData, section_header[i].SizeOfRawData);

		current_section_address = reinterpret_cast<void*>(uintptr_t(start_of_executable) //
		                                                  + section_header[i].Misc.VirtualSize);
	}
	return start_of_executable;
}

inline uintptr_t PELoader::align_to(uintptr_t size, size_t alignment)
{
	if (!size)
		return 0;
	else
		return size + alignment - (size % alignment);
}
