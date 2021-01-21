#include "ELFParser.h"

ELFParser::ELFParser(char* file, size_t size) : m_file{file}, m_size{size}
{
	m_elf_header = reinterpret_cast<elf32_hdr*>(file);
	m_program_headers_array = reinterpret_cast<elf32_phdr*>(m_file + m_elf_header->e_phoff);
	m_section_headers_array = reinterpret_cast<elf32_shdr*>(m_file + m_elf_header->e_shoff);
	m_programs_number = m_elf_header->e_phnum;
	m_sections_number = m_elf_header->e_shnum;

	if (m_elf_header->e_shstrndx != SHN_UNDEF &&
	    m_section_headers_array[m_elf_header->e_shstrndx].sh_type == SHT_STRTAB) {
		m_string_table = m_file + m_section_headers_array[m_elf_header->e_shstrndx].sh_offset;
	} else {
		m_string_table = nullptr;
	}
}

elf32_hdr& ELFParser::elf_header()
{
	return *m_elf_header;
}

elf32_phdr& ELFParser::program_header_by_index(size_t index)
{
	return m_program_headers_array[index];
}

elf32_shdr& ELFParser::section_header_by_index(size_t index)
{
	return m_section_headers_array[index];
}

StringView ELFParser::lookup_for_string(size_t index_in_string_table)
{
	return m_string_table + index_in_string_table;
}

size_t ELFParser::sections_number()
{
	return m_elf_header->e_shnum;
}

size_t ELFParser::programs_number()
{
	return m_elf_header->e_phnum;
}

size_t ELFParser::entry_point()
{
	return m_elf_header->e_entry;
}