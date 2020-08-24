#include "pe.h"
#include "Lib/stdlib.h"
#include "VirtualMemory/memory.h"
#include "utils/ErrorCodes.h"

Result<uintptr_t> PELoader::load(const char* file, size_t size)
{
	ASSERT(file);
	ASSERT(size);
	// validations
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
		const uint32_t section_va = section_header[i].VirtualAddress;
		const uint32_t section_rd = section_header[i].PointerToRawData;
		const uint32_t section_raw_size = section_header[i].SizeOfRawData;
		const uint32_t section_vir_size = section_header[i].Misc.VirtualSize;

		if (!section_va || !section_rd || !section_vir_size)
			return ResultError(ERROR_INVALID_EXECUTABLE);

		if ((section_rd + section_raw_size) > size)
			return ResultError(ERROR_INVALID_EXECUTABLE);
	}

	if (!nt_header->OptionalHeader.AddressOfEntryPoint)
		return ResultError(ERROR_INVALID_EXECUTABLE);

	// Load PE sections
	void* start_of_executable = load_pe_sections(file, nt_header);
	if (!start_of_executable)
		return ResultError(ERROR_LOADING_EXECUTABLE);

	return uintptr_t(start_of_executable) + nt_header->OptionalHeader.AddressOfEntryPoint;
}

void* PELoader::load_pe_sections(const char* file, const IMAGE_NT_HEADERS32* nt_header)
{
	const IMAGE_SECTION_HEADER* section_header = reinterpret_cast<const IMAGE_SECTION_HEADER*>(nt_header + 1);
	const uint32_t section_alignment = nt_header->OptionalHeader.SectionAlignment;
	// Load headers
	void* start_of_executable = Memory::alloc(reinterpret_cast<void*>(nt_header->OptionalHeader.ImageBase),
	                                          nt_header->OptionalHeader.SectionAlignment, MEMORY_TYPE::WRITABLE);
	if (!start_of_executable)
		return nullptr;

	memset(start_of_executable, 0, section_alignment);
	memcpy(start_of_executable, file, nt_header->OptionalHeader.SizeOfHeaders);

	// load sections
	for (size_t i = 0; i < nt_header->FileHeader.NumberOfSections; i++) {
		const uint32_t section_va = section_header[i].VirtualAddress;
		const uint32_t section_rd = section_header[i].PointerToRawData;
		const uint32_t section_raw_size = section_header[i].SizeOfRawData;
		const uint32_t section_vir_size = align_to(section_header[i].Misc.VirtualSize, section_alignment);
		void* current_section_address = reinterpret_cast<void*>(uintptr_t(start_of_executable) + section_va);

		current_section_address = Memory::alloc(current_section_address, section_vir_size, MEMORY_TYPE::WRITABLE);
		if (!current_section_address)
			return nullptr;

		memcpy(current_section_address, file + section_rd, section_raw_size);
	}
	return start_of_executable;
}

inline uintptr_t PELoader::align_to(uintptr_t size, size_t alignment)
{
	if (size == 0)
		return alignment;
	else if ((size % alignment) == 0)
		return size;
	else
		return size + alignment - (size % alignment);
}
