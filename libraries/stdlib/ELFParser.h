#include "ELF.h"
#include "StringView.h"
#include "Types.h"

class ELFParser
{
  private:
	char* m_file;
	size_t m_size;
	elf32_hdr* m_elf_header;
	elf32_phdr* m_program_headers_array;
	elf32_shdr* m_section_headers_array;
	size_t m_programs_number;
	size_t m_sections_number;
	char* m_string_table;

  public:
	ELFParser(char* file, size_t size);

	elf32_hdr& elf_header();
	elf32_phdr& program_header_by_index(size_t index);
	elf32_shdr& section_header_by_index(size_t index);
	StringView lookup_for_string(size_t index_in_string_table);
	size_t sections_number();
	size_t programs_number();
	size_t entry_point();
	template <typename T> void for_each_program_header(T function) {}
	template <typename T> void for_each_section_header(T function) {}
};
