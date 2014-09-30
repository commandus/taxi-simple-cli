#include "taxi-simple-cli.h"

#ifdef WIN32
#include <tchar.h>
#include "argvcharsetconv.h"

int _tmain(int argc, TCHAR** argv)
{
	char **argv_utf8 = argvConv(argc, argv);
	int r = doCmd(argc, argv_utf8);
	argvFree(argc, argv_utf8);
	return r;
}

#else

int main(int argc, char** argv)
{
        int r = doCmd(argc, argv);
        return r;
}

#endif
