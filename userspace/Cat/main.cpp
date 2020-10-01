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
		printf("Cannot open the file.");
		return result;
	}

	FileInfo info;
	QueryFileInformation(fd, &info);
	if ((result = GetLastError())) {
		printf("Cannot read the file.");
		CloseHandle(fd);
		return result;
	}

	char* buff = new char[info.size + 1];
	memset(buff, 0, info.size + 1);
	ReadFile(fd, buff, info.size);
	if ((result = GetLastError())) {
		printf("Cannot read the file.");
		CloseHandle(fd);
		return result;
	}

	printf(buff);

	delete[] buff;
	CloseHandle(fd);

	return 0;
}