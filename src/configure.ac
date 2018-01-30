#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pca-face-recog], [0.1], [panderan@163.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([face_recognition.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([opencv], [opencv])
PKG_CHECK_MODULES([glib2], [glib-2.0])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset sqrt])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

