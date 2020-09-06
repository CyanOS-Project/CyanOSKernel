#include <libsystem/Systemcalls/Systemcalls.h>
#include <libsystem/Types.h>

int _main()
{
	char buff[1000];
	int ds = OpenFile("/Tar/Drivers/nop_loop.exe", 0, 1);
	ReadFile(ds, buff, 0x10);
	CloseFile(ds);

	while (true) {
	}
	return 0;
}