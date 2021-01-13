
#include "../Systemcalls/Systemcalls.h"
#include "../Types.h"

extern "C" int main(int argc, const char* argv[]);

extern "C" int startup()
{
	char *path, *arg;
	asm("movl %%gs:0x4,%0" : "=r"(path));
	asm("movl %%gs:0x8,%0" : "=r"(arg));
	const char* main_args[2];
	main_args[0] = path;
	main_args[1] = arg;
	main(2, main_args);
	TerminateProcess(-1, 0);
	return 1;
}