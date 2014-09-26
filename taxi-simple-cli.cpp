#include "taxi-simple-cli.h"
#include "argvcharsetconv.h"

int _tmain(int argc, TCHAR** argv)
{
	char **argv_utf8 = argvConv(argc, argv);
	int r = doCmd(argc, argv_utf8);
	argvFree(argc, argv_utf8);
	return r;
}

