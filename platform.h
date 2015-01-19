#ifndef _PLATFORM_H_
#define _PLATFORM_H_	1

#include <stdint.h>

#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
// https://gcc.gnu.org/projects/cxx0x.html GCC 4.5 New wording for C++0x lambdas
#if GCC_VERSION > 40500
	#define USE_LAMBDA	1
#elif GCC_VERSION > 40600
	#define USE_STD_END	1
#endif
#else
#define USE_LAMBDA	1
#define USE_STD_END	1
#endif

#ifndef USE_STD_END
// older gcc C11 experimental not supported
namespace std {
template<class _Ty,
	size_t _Size> inline
	_Ty *end(_Ty (&_Array)[_Size])
	{	// get end of array
	return (&_Array[0] + _Size);
	}
}
#endif
#endif