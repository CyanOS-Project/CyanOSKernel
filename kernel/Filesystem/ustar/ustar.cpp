#include "ustar.h"
#include "utils/Stack.h"

TarFS::TarFS(void* tar_address, size_t size) :
    m_tar_address(static_cast<TarHeader*>(tar_address)),
    m_root("root", 0, nullptr)
{
	ASSERT(tar_address);
	ASSERT(strncmp(m_tar_address->magic, "ustar", 5) == 0)
	parse_ustar(size);
}

TarFS::~TarFS()
{
}

FSNode& TarFS::root_node()
{
	return m_root;
}

INode& TarFS::add_child_node(INode& parent, const char* name, const size_t size, char* data)
{
	return parent.m_children.emplace_back(name, size, data);
}

void TarFS::parse_ustar(size_t size)
{
	TarHeader* tar_parser = m_tar_address;
	char last_path_element[MAX_FILE_NAME];
	//
	Stack<INode*> directories(10);
	directories.queue(&m_root);

	while (uintptr_t(tar_parser) < (uintptr_t(m_tar_address) + size)) {
		if (!tar_parser->name[0])
			break;

		remove_tailing_slash(tar_parser->name);
		PathParser parser(tar_parser->name);
		parser.get_element(parser.path_element_count() - 1, last_path_element, MAX_FILE_NAME);

		if (parser.path_element_count() > 1) {
			char parent_path[MAX_FILE_NAME];
			parser.get_element(parser.path_element_count() - 2, parent_path, MAX_FILE_NAME);
			while (strcmp(parent_path, directories.back()->m_filename) && directories.size() > 1) {
				directories.dequeue();
			}
		} else {
			while (directories.size() > 1) {
				directories.dequeue();
			}
		}

		auto& new_node = add_child_node(*directories.back(), last_path_element, octal_to_decimal(tar_parser->size),
		                                reinterpret_cast<char*>(tar_parser + 1));
		if (tar_parser->typeflag == USTARFileType::DIRECTORY) {
			directories.queue(&new_node);
		}
		const uintptr_t aligned_size = align_to(octal_to_decimal(tar_parser->size), TAR_ALIGNMENT);
		tar_parser = (TarHeader*)(uintptr_t(tar_parser + 1) + aligned_size);
	}
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
	else if ((size % alignment) == 0)
		return size;
	else
		return size + alignment - (size % alignment);
}

void TarFS::remove_tailing_slash(char* path)
{
	size_t last_index = strlen(path) - 1;
	if (path[last_index] == SPLITER)
		path[last_index] = 0;
}
