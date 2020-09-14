#include <systemlib/Systemcalls/Systemcalls.h>
#include <systemlib/Types.h>
#include <systemlib/iostream/iostream.h>

void main()
{
	printf("Hello I'm cute child\n");
	printf("I'm going to die now :(\n");
	printf("Bye...\n");
	TerminateProcess(-1, 0);
	printf("Am i in heaven?, I shouldn't be alive!\n");
	while (true) {
	}
}