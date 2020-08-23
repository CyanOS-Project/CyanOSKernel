#include "ustar.h"
#include "Tasking/ScopedLock.h"
#include "utils/ErrorCodes.h"
#include "utils/Stack.h"
#include "utils/stl.h"

UniquePointer<FSNode> TarFS::alloc(void* tar_address, size_t size)
{
	return UniquePointer<FSNode>(new TarFS(tar_address, size));
}

TarFS::TarFS(void* tar_address, size_t size) :
    INode{"Tar", NodeType::Root, 0, nullptr},
    m_tar_address(static_cast<TarHeader*>(tar_address))
{
	ASSERT(tar_address);
	ASSERT(strncmp(m_tar_address->magic, "ustar", 5) == 0)
	parse_ustar(size);
}

TarFS::~TarFS()
{
}

INode& TarFS::add_child_node(INode& parent, const StringView& name, char type, const size_t size, char* data)
{
	FSNode::NodeType node_type;
	if (type == USTARFileType::DIRECTORY) {
		node_type = FSNode::NodeType::Folder;
	} else {
		node_type = FSNode::NodeType::File;
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

String TarFS::regulate_path(const char* path)
{
	String full_path(path);
	full_path.insert(0, "/");
	if (full_path[full_path.length() - 1] == '/') {
		full_path.pop_back();
	}

	return full_path;
}

Result<FSNode&> TarFS::create(const StringView& name, OpenMode mode, OpenFlags flags)
{
	UNUSED(name);
	UNUSED(mode);
	UNUSED(flags);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> TarFS::open(OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);

	return ResultError(ERROR_SUCCESS);
}

Result<void> TarFS::close()
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> TarFS::read(void* buff, size_t offset, size_t size)
{
	UNUSED(buff);
	UNUSED(offset);
	UNUSED(size);

	return ResultError(ERROR_SUCCESS);
}

Result<void> TarFS::write(const void* buff, size_t offset, size_t size)
{
	UNUSED(buff);
	UNUSED(offset);
	UNUSED(size);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<bool> TarFS::can_read()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<bool> TarFS::can_write()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> TarFS::remove()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> TarFS::mkdir(const StringView& name, int flags, int access)
{
	UNUSED(name);
	UNUSED(flags);
	UNUSED(access);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> TarFS::link(FSNode& node)
{
	UNUSED(node);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> TarFS::unlink(FSNode& node)
{
	UNUSED(node);
	return ResultError(ERROR_INVALID_OPERATION);
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
