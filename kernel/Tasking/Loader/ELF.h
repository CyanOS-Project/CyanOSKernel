#include <ELFParser.h>
#include <Result.h>
#include <Types.h>

struct ExecutableInformation {
	uintptr_t entry_point;
	uintptr_t* constructors_array;
	size_t constructors_array_count;
};

class ELFLoader
{
  private:
	static bool load_segments(BufferView file, const ELFParser& elf);

  public:
	static Result<ExecutableInformation> load(BufferView file);
};
