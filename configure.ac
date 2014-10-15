AC_PREREQ([2.63])
AC_INIT([taxi-simple-cli], [1.0], [support@commandus.com])
AM_INIT_AUTOMAKE([1.11 foreign no-define])
AC_CONFIG_SRCDIR([client.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# Checks for libraries.
AC_CHECK_HEADERS([argtable2.h], [], AC_MSG_ERROR([Could not find argtable2.h. Try $ ./configure CFLAGS='-Iyour-argtable2-include-path]))
AC_CHECK_LIB(argtable2, arg_parse, [], AC_MSG_ERROR([Could not find libargtable2. Try $ ./configure LDFLAGS='-Lyour-argtable2-lib-path']))
#AC_CHECK_HEADERS([thrift/Thrift.h], [], AC_MSG_ERROR([Could not find thrift/Thrift.h. Try $ ./configure CFLAGS='-Iyour-thrift-include-path]))
AC_CHECK_LIB([thrift],[_init], [], AC_MSG_ERROR([Could not find libthrift. Try $ ./configure LDFLAGS='-Lyour-thrift-lib-path']))
AC_CHECK_HEADER([curl/curl.h], [], AC_MSG_ERROR([Could not find curl/curl.h. Try $ ./configure CFLAGS='-Iyour-curl-include-path])) 
AC_CHECK_LIB(curl, curl_easy_init,  [], AC_MSG_ERROR([Could not find libcurl. Try $ ./configure LDFLAGS='-Lyour-curl-lib-path'])) 
AC_CHECK_HEADER([libconfig.h], [], AC_MSG_ERROR([Could not find libconfig.h++. Try $ ./configure CFLAGS='-Iyour-libconfig-include-path]))
AC_CHECK_LIB([config++], main,  [], AC_MSG_ERROR([Could not find libconfig++. Try $ ./configure LDFLAGS='-Lyour-config++-lib-path']))
AC_CHECK_HEADERS([fcntl.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset socket select])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
