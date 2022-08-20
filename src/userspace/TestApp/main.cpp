#include <systemlib/Systemcalls/Systemcalls.h>
#include <systemlib/Types.h>
#include <systemlib/iostream/iostream.h>

void main(int argc, const char* argv[])
{
	UNUSED(argc);

	printf("Hello I'm cute child\n");
	printf("My path is: ");
	printf(argv[0]);
	printf("\nMy argument is: ");
	printf(argv[1]);
	printf("\nI'm going to die now :(\n");
	printf("Bye...\n");
	TerminateProcess(-1, 0);
	printf("Am i in heaven?, I shouldn't be alive!\n");
	while (true) {
	}
}