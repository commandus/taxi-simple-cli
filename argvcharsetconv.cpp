#include "argvcharsetconv.h"
#include <sys/stat.h>
#include <string.h>

#ifdef _WIN32
std::vector<std::wstring> argv2vector(int argc, TCHAR** argv)
{
	return std::vector<std::wstring>(argv + 1, argv + argc);
}

void argvFree(int argc, char** argv_utf8)
{
	free(argv_utf8);
}

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
	size_t sz = len + argc + 1;	// plus terminator zero
	size_t ptrsize = (argc + 1) * sizeof(char**);	// plus last zero 
	size_t bufsize = ptrsize + sz;

	char **ptr = (char **)malloc(bufsize);
	char *buffer = (char *) ptr + ptrsize;
	memset(ptr, 0, bufsize);	// for trailing zeroes
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
#endif

bool fileexists(const std::string& fn)
{
	struct stat buffer;   
	return (stat(fn.c_str(), &buffer) == 0); 
}

std::vector<std::string> argv2vector(int argc, char** argv)
{
	return std::vector<std::string>(argv, argv + argc);
}

size_t vector2argv(std::vector<std::string> value, int *argc, char*** argv)
{
	size_t len = 0L;
	*argc = value.size();
	if (!*argc)
	{
		*argv = NULL;
		return 0L;
	}

	for (std::vector<std::string>::iterator i(value.begin()); i != value.end(); ++i)
	{
		len += i->length();
		len++;	// trailing /0
	}
	// allocate char buffer and ptr array
	size_t ptrsize = (*argc + 1) * sizeof(char**);	// plus last zero 
	size_t bufsize = ptrsize + len;
	char **ptr = (char **)malloc(bufsize);
	char *buffer = (char *) ptr + ptrsize;
	*argv = ptr;
	memset(ptr, 0, bufsize);	// for trailing zeroes
	
	// convert

	char *pb = buffer;
	char **cptr = ptr;
	for (std::vector<std::string>::iterator i(value.begin()); i != value.end(); ++i)
	{
		*cptr =  pb;
		size_t l = i->length();
		memcpy(pb, i->c_str(), l);
		pb += l;
		pb++;	// trailing /0
		cptr++;
	}
	return bufsize;
}

