#include <Clib.h>
#include <ErrorCodes.h>
#include <systemlib/Systemcalls/Systemcalls.h>
#include <systemlib/Types.h>
#include <systemlib/iostream/iostream.h>

int main(int argc, const char* argv[])
{
	if (argc <= 1) {
		return 1;
	}
	int result = 0;

	Handle fd = OpenFile(argv[2], OM_WRITE | OM_READ, OF_OPEN_EXISTING);
	if ((result = GetLastError())) {
		return result;
	}

	FileInfo info;
	QueryFileInformation(fd, &info);
	if ((result = GetLastError())) {
		CloseHandle(fd);
		return result;
	}

	char* buff = new char[info.size];
	ReadFile(fd, buff, info.size);
	if ((result = GetLastError())) {
		CloseHandle(fd);
		return result;
	}
	delete[] buff;

	CloseHandle(fd);
	TerminateProcess(-1, 0);

	return 0;
}