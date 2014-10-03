#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([taxi-simple-cli], [1.0], [support@commandus.com])
AM_INIT_AUTOMAKE([1.12 foreign no-define])
AC_CONFIG_SRCDIR([client.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# Checks for libraries.
# FIXME: Replace `main' with a function in `-largtable2':
AC_CHECK_LIB([argtable2], [main])
# FIXME: Replace `main' with a function in `-lthrift':
AC_CHECK_LIB([thrift], [main])

AC_CHECK_LIB([curl], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
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
