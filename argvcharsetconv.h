#include <vector>
#include <string>
#ifdef _WIN32
#include <tchar.h>
// after argvConv(c, v) call argvFree(v)!
char** argvConv(int argc, TCHAR** argv);
void argvFree(int argc, char** argv_utf8);
std::vector<std::wstring> argv2vector(int argc, TCHAR** argv);
#endif

std::vector<std::string> argv2vector(int argc, char** argv);
size_t vector2argv(std::vector<std::string> value, int *argc, char*** argv);

bool fileexists(const std::string& fn);