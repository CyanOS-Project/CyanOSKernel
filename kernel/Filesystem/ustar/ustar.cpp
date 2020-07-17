#include "ustar.h"

TarFS::TarFS(void* tar_address) : m_tar_address(static_cast<TarHeader*>(tar_address)), m_root("root", 0, nullptr)
{
	ASSERT(tar_address);
	parse_ustar();
}

TarFS::~TarFS()
{
}

Result<FSNode&> TarFS::get_root_node()
{
	return m_root;
}

/*Result<INode&> TarFS::add_child_node(INode& parent, const INode& child)
{
    return parent.m_children->push_back(child);
}*/
Result<void> TarFS::parse_ustar()
{

	if (strncmp(m_tar_address->magic, "ustar", 5))
		return ResultError(ERROR_INVALID_PARAMETERS);

	TarHeader* tar_parser = m_tar_address;
	INode* last_parent = &m_root;
	char last_element[MAX_FILE_NAME];
	while (tar_parser->name[0]) {
		remove_tailing_slash(tar_parser->name);
		PathParser parser(tar_parser->name);
		parser.get_element(parser.path_element_count() - 1, last_element, MAX_FILE_NAME);
		auto& new_node = last_parent->m_children->emplace_back(last_element, octal_to_decimal(tar_parser->size),
		                                                       reinterpret_cast<char*>(tar_parser + 1));
		if (tar_parser->typeflag == USTARFileType::DIRECTORY) {
			last_parent = &new_node;
		}
		uintptr_t aligned_size = align_to(octal_to_decimal(tar_parser->size), TAR_ALIGNMENT);
		tar_parser = (TarHeader*)(uintptr_t(tar_parser + 1) + aligned_size);
	}
	return ResultError(ERROR_UNDEFINED);
}

size_t TarFS::octal_to_decimal(const char* octal)
{
	size_t size = 0;
	unsigned current_multiplier = 1;
	for (size_t i = TAR_OCTAL_SIZE_LEN - 1; i > 0; i--) {
		size += (octal[i - 1] - '0') * current_multiplier;
		current_multiplier *= 8;
	}
	return size;
}

inline uintptr_t TarFS::align_to(uintptr_t size, unsigned alignment)
{
	if (!size)
		return 0;
	else
		return size + alignment - (size % alignment);
}

void TarFS::remove_tailing_slash(char* path)
{
	size_t last_index = strlen(path) - 1;
	if (path[last_index] == SPLITER)
		path[last_index] = 0;
}
