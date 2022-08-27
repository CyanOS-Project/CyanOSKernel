#pragma once

#include "elf.h"
#include "string_view.h"
#include "types.h"
#include <buffer_view.h>

class ELFParser
{
  private:
	BufferView m_file;
	const elf32_hdr* m_elf_header;
	const elf32_phdr* m_program_headers_array;
	const elf32_shdr* m_section_headers_array;
	size_t m_programs_number;
	size_t m_sections_number;
	const char* m_string_table;

	const char* find_string_table() const;

  public:
	ELFParser(BufferView file);
	bool is_valid() const;
	const elf32_hdr& elf_header() const;
	const elf32_phdr& program_header_by_index(size_t index) const;
	const elf32_shdr& section_header_by_index(size_t index) const;
	const elf32_shdr& section_header_by_name(StringView name) const;
	const elf32_shdr& section_header_by_type(u32 type) const;
	StringView lookup_for_string(size_t index_in_string_table) const;
	size_t sections_number() const;
	size_t programs_number() const;
	uintptr_t entry_point() const;
	template <typename T> void for_each_program_header(T function) const {}
	template <typename T> void for_each_section_header(T function) const {}
};
