#include <ELFParser.h>
#include <Result.h>
#include <Types.h>

class ELFLoader
{
  private:
	static bool load_segments(const char* file, const ELFParser& elf);

  public:
	static Result<uintptr_t> load(const char* file, size_t size);
};
