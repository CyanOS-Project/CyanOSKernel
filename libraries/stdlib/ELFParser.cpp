#include "ELFParser.h"
#include "Assert.h"
#include "Clib.h"

ELFParser::ELFParser(const char* file, size_t size) :
    m_file{file},
    m_size{size},
    m_elf_header{reinterpret_cast<const elf32_hdr*>(file)}
{
	ASSERT(m_file);
	if (!is_valid()) {
		return;
	}

	m_program_headers_array = reinterpret_cast<const elf32_phdr*>(m_file + m_elf_header->e_phoff);
	m_section_headers_array = reinterpret_cast<const elf32_shdr*>(m_file + m_elf_header->e_shoff);
	m_programs_number = m_elf_header->e_phnum;
	m_sections_number = m_elf_header->e_shnum;
	m_string_table = find_string_table();
}

bool ELFParser::is_valid() const
{
	if (!strncmp((char*)m_elf_header->e_ident, ELFMAG, SELFMAG)) {
		return true;
	} else {
		return false;
	}
}

const elf32_hdr& ELFParser::elf_header() const
{
	return *m_elf_header;
}

const elf32_phdr& ELFParser::program_header_by_index(size_t index) const
{
	return m_program_headers_array[index];
}

const elf32_shdr& ELFParser::section_header_by_index(size_t index) const
{
	return m_section_headers_array[index];
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
		return m_file + m_section_headers_array[m_elf_header->e_shstrndx].sh_offset;
	} else {
		return nullptr;
	}
}