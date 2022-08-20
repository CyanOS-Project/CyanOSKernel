#include "ELFParser.h"
#include "Assert.h"
#include "Clib.h"

ELFParser::ELFParser(BufferView file) : m_file{file}, m_elf_header{reinterpret_cast<const elf32_hdr*>(file.ptr())}
{
	if (!is_valid()) {
		return;
	}

	m_program_headers_array = reinterpret_cast<const elf32_phdr*>(m_file.ptr() + m_elf_header->e_phoff);
	m_section_headers_array = reinterpret_cast<const elf32_shdr*>(m_file.ptr() + m_elf_header->e_shoff);
	m_programs_number = m_elf_header->e_phnum;
	m_sections_number = m_elf_header->e_shnum;
	m_string_table = find_string_table();
}

bool ELFParser::is_valid() const
{
	return !strncmp((char*)m_elf_header->e_ident, ELFMAG, SELFMAG);
}

const elf32_hdr& ELFParser::elf_header() const
{
	return *m_elf_header;
}

const elf32_phdr& ELFParser::program_header_by_index(size_t index) const
{
	if (index < m_sections_number) {
		return m_program_headers_array[index];
	} else {
		return m_program_headers_array[SHN_UNDEF];
	}
}

const elf32_shdr& ELFParser::section_header_by_index(size_t index) const
{
	if (index < m_sections_number) {
		return m_section_headers_array[index];
	} else {
		return m_section_headers_array[SHN_UNDEF];
	}
}
const elf32_shdr& ELFParser::section_header_by_name(StringView name) const
{
	for (size_t i = 0; i < m_sections_number; i++) {
		auto& section = section_header_by_index(i);
		if (name == lookup_for_string(section.sh_name)) {
			return section;
		}
	}
	return m_section_headers_array[SHN_UNDEF];
}

const elf32_shdr& ELFParser::section_header_by_type(u32 type) const
{
	for (size_t i = 0; i < m_sections_number; i++) {
		auto& section = section_header_by_index(i);
		if (type == section.sh_type) {
			return section;
		}
	}
	return m_section_headers_array[SHN_UNDEF];
}

StringView ELFParser::lookup_for_string(size_t index_in_string_table) const
{
	return m_string_table + index_in_string_table;
}

size_t ELFParser::sections_number() const
{
	return m_elf_header->e_shnum;
}

size_t ELFParser::programs_number() const
{
	return m_elf_header->e_phnum;
}

uintptr_t ELFParser::entry_point() const
{
	return m_elf_header->e_entry;
}

const char* ELFParser::find_string_table() const
{
	if (m_elf_header->e_shstrndx != SHN_UNDEF &&
	    m_section_headers_array[m_elf_header->e_shstrndx].sh_type == SHT_STRTAB) {
		return (const char*)(m_file.ptr() + m_section_headers_array[m_elf_header->e_shstrndx].sh_offset);
	} else {
		return nullptr;
	}
}