#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([inere], [0.4.0], [egamez@edisson.com.mx])
AC_CONFIG_SRCDIR([src/rfc.c])
AC_CONFIG_HEADERS([include/inere/config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

# For libtool
LT_INIT
AC_PROG_LIBTOOL

# For automake
AM_INIT_AUTOMAKE

# Checks for header files.
AC_CHECK_HEADERS([time.h locale.h stddef.h stdlib.h string.h iconv.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([localtime_r memset setlocale strdup strrchr strsep strcasecmp iconv])

# Checks for extra libraries.


# Options to build the other subpackages
ENVIA_ENABLED=no
AC_ARG_ENABLE([envia],
	[AS_HELP_STRING([--enable-envia],
		[Compila el subpaquete envia])],
	[
		case "${enableval}" in
			yes)	ENVIA_ENABLED=true
				AC_CONFIG_SUBDIRS([envia])
				;;
			no)	ENVIA_ENABLED=false ;;
			*)	AC_MSG_ERROR([bad value ${enableval} for --enable-envia]) ;;
		esac
	],
	[
		ENVIA_ENABLED=false
	])
AM_CONDITIONAL([ENVIA_ENABLED], [ test x$ENVIA_ENABLED = xtrue ])


CFDI_ENABLED=no
AC_ARG_ENABLE([cfdi],
	[AS_HELP_STRING([--enable-cfdi],
		[Compila el subpaquete cfdi])],
	[
		case "${enableval}" in
			yes)	CFDI_ENABLED=true
				AC_CONFIG_SUBDIRS([cfdi])
				;;
			no)	CFDI_ENABLED=false ;;
			*)	AC_MSG_ERROR([valor inapropiado ${enableval} para --enable-cfdi]) ;;
		esac
	],
	[
		CFDI_ENABLED=false
	])
AM_CONDITIONAL([CFDI_ENABLED], [ test x$CFDI_ENABLED = xtrue ])


# Now capture the input charset
DEFAULT_CHARSET="UTF-8"
if test "$LANG" ; then
    DEFAULT_CHARSET=`echo $LANG | cut -d '.' -f 2 | cut -d '@' -f 1`
    AC_MSG_NOTICE([$DEFAULT_CHARSET character set used for character conversions.])
else
    AC_MSG_NOTICE([Using utf-8 as default character set for conversions.])
fi
AC_DEFINE_UNQUOTED([INERE_LOCAL_CHARSET], ["$DEFAULT_CHARSET"], [Character set to be used for character conversions to UTF-8l])


AC_CONFIG_FILES([inere-config], [chmod +x inere-config])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
		 examples/Makefile
		 python/setup.py])
AC_OUTPUT
