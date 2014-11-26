#include <vector>
#include <string>
#include "platform.h"
#ifdef _WIN32
#include <tchar.h>

// after argvConv(c, v) call argvFree(v)!
char** argvConv(int argc, TCHAR** argv);
void argvFree(int argc, char** argv_utf8);
std::vector<std::wstring> argv2vector(int argc, TCHAR** argv);
#endif

std::vector<std::string> argv2vector(int argc, char** argv);
size_t vector2argv(std::vector<std::string> value, int *argc, char*** argv);
int done(void** argtable);

bool fileexists(const std::string& fn);

/**
	Get config file name from the command line
*/
bool parseGetConfigFile(int argc, char** argv, std::string &ret);
/*
	Try to find out DEF_CONFIG in ".", "~", "/usr/local/etc", "/etc/" 
*/
bool findConfigInSystemDir(std::string &ret);
