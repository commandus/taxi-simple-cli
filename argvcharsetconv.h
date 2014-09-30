#ifndef _WIN32
	error Windows only
#endif
#include <tchar.h>

char** argvConv(int argc, TCHAR** argv);
void argvFree(int argc, char** argv_utf8);
