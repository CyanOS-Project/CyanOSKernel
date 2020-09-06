#include "TarFS.h"
#include "Tasking/ScopedLock.h"
#include "Utils/Algorithms.h"
#include "Utils/ErrorCodes.h"
#include "Utils/Stack.h"

UniquePointer<FSNode> TarFS::alloc(const StringView& name, void* tar_address, size_t size)
{
	return UniquePointer<FSNode>(new TarFS(name, tar_address, size));
}

TarFS::TarFS(const StringView& name, void* tar_address, size_t size) :
    INode{name, NodeType::Root, 0, nullptr},
    m_tar_address(static_cast<TarHeader*>(tar_address))
{
	ASSERT(tar_address);
	ASSERT(strncmp(m_tar_address->magic, "ustar", 5) == 0);
	parse_ustar(size);
}

TarFS::~TarFS() {}

INode& TarFS::add_child_node(INode& parent, const StringView& name, char type, const size_t size, char* data)
{
	NodeType node_type;
	if (type == USTARFileType::DIRECTORY) {
		node_type = NodeType::Folder;
	} else {
		node_type = NodeType::File;
	}

	return parent.m_children.emplace_back(name, node_type, size, data);
}

void TarFS::parse_ustar(size_t size)
{
	TarHeader* tar_parser = m_tar_address;
	Stack<INode*> directories(10);
	directories.queue(this);

	while (uintptr_t(tar_parser) < (uintptr_t(m_tar_address) + size)) {
		if (!tar_parser->name[0])
			break;
		String path = regulate_path(tar_parser->name);
		PathParser parser = PathParser(StringView(path));

		if (parser.count() > 1) {
			while (directories.back()->m_name != (parser.element(parser.count() - 2)) && (directories.size() > 1)) {
				directories.dequeue();
			}
		} else {
			while (directories.size() > 1) {
				directories.dequeue();
			}
		}

		auto& new_node = add_child_node(
		    *directories.back(),                //
		    parser.element(parser.count() - 1), //
		    tar_parser->typeflag,               //
		    octal_to_decimal(tar_parser->size), //
		    tar_parser->typeflag != USTARFileType::DIRECTORY ? reinterpret_cast<char*>(tar_parser + 1) : nullptr);

		if (tar_parser->typeflag == USTARFileType::DIRECTORY) {
			directories.queue(&new_node);
		}
		const uintptr_t aligned_size = align_to(octal_to_decimal(tar_parser->size), TAR_ALIGNMENT, false);
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

String TarFS::regulate_path(const char* path)
{
	String full_path(path);
	full_path.insert(0, "/");
	if (full_path[full_path.length() - 1] == '/') {
		full_path.pop_back();
	}

	return full_path;
}

Result<void> TarFS::open(UNUSEDARG FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<FSNode&> TarFS::dir_lookup(const StringView& file_name)
{
	ScopedLock local_lock(m_lock);
	for (auto& i : m_children) {
		if (i.m_name == file_name) {
			return i;
		}
	}
	return ResultError(ERROR_FILE_DOES_NOT_EXIST);
}

Result<StringView> TarFS::dir_query(size_t index)
{
	ScopedLock local_lock(m_lock);
	if (index >= m_children.size())
		return ResultError(ERROR_INVALID_PARAMETERS);

	// FIXME: it's not efficeint way to iterate them all again;
	auto itr = m_children.begin();
	while (index--) {
		itr++;
	}
	return StringView(itr->m_name);
}