#include "argvcharsetconv.h"
#include <string>

char** argvConv(int argc, TCHAR** argv)
{
	// count bytes
	size_t len = 0;
	TCHAR **p;
	p = argv;
	for (int i = 0; i < argc; i++)
	{
		std::wstring ws(*p);
		std::string s(ws.begin(), ws.end());
		len += s.length();
		p++;
	}

	// allocate char buffer and ptr array
	size_t sz = len + argc + 1;	// plus terminator zeroes
	char *buffer = (char *)malloc(sz);
	memset(buffer, 0, sz);	// for trailing zeroes
	size_t ptrsize = argc * sizeof(char**)+1;
	char **ptr = (char **)malloc(ptrsize);
	memset(ptr, 0, ptrsize);	// last NULL means terminator

	// convert
	p = argv;
	char *pb = buffer;
	char **cptr = ptr;
	for (int i = 0; i < argc; i++)
	{
		std::wstring ws(*p);
		std::string s(ws.begin(), ws.end());
		len = s.length();
		*cptr = pb;
		memcpy(pb, s.c_str(), len);
		pb += len;
		pb++;	// trailing /0

		cptr++;
		p++;
	}
	return ptr;
}

void argvFree(int argc, char** argv_utf8)
{
	free(*argv_utf8);
	free(argv_utf8);
}
