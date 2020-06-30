#include "ustar.h"

TarFS::TarFS(void* tar_address) : m_tar_address((TarHeader*)tar_address)
{
}

char* TarFS::read_file(const char* path)
{
	TarHeader* file_header = file_search(path);
	if (!file_header)
		return nullptr;
	return (char*)(file_header + 1);
}

size_t TarFS::get_file_size(const char* path)
{
	TarHeader* file_header = file_search(path);
	if (!file_header)
		return 0;
	return octal_to_decimal(file_header->size);
}

TarHeader* TarFS::file_search(const char* path)
{
	size_t path_len = strlen(path) + 1;
	if (path_len > MAX_FILE_NAME)
		return nullptr;

	if (strncmp(m_tar_address->magic, "ustar", 5))
		return nullptr;

	TarHeader* tar_parser = m_tar_address;
	while (tar_parser->name[0]) {
		if (strcmp(path, tar_parser->name) == 0) {
			return tar_parser;
		}
		uintptr_t aligned_size = align_to(octal_to_decimal(tar_parser->size), TAR_ALIGNMENT);
		tar_parser = (TarHeader*)(uintptr_t(tar_parser + 1) + aligned_size);
	}
	return nullptr;
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
	return size + alignment - (size % alignment);
}
