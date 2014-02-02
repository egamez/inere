#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([inere], [0.1.1], [egamez@edisson.com.mx])
AC_CONFIG_SRCDIR([src/rfc.c])
AC_CONFIG_HEADERS([include/inere/config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# For libtool
AC_PROG_LIBTOOL

# For automake
AM_INIT_AUTOMAKE

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h iconv.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([strsep strcasecmp iconv])


# Checks for extra libraries.

# xml2
AC_PATH_PROGS(XML2_CONFIG, xml2-config)
if test -z "$XML2_CONFIG" || test ! -r "$XML2_CONFIG"; then
        AC_MSG_ERROR([
XML2 required.
])
fi

with_xml2_include=`$XML2_CONFIG --prefix`
AC_MSG_NOTICE([using XMl2 headers at $with_xml2_include])
with_xml2_lib=`$XML2_CONFIG --prefix`
AC_MSG_NOTICE([using XML2 libraries at $with_xml2_lib])
XML2_PREFIX=`$XML2_CONFIG --prefix`
AC_SUBST(XML2_PREFIX)

XML2_LIB=`$XML2_CONFIG --libs`
AC_SUBST(XML2_LIB)
XML2_INCLUDE=`$XML2_CONFIG --cflags`
AC_SUBST(XML2_INCLUDE)

# xslt
AC_PATH_PROG(XSLT_CONFIG, xslt-config)
if test -z "$XSLT_CONFIG" || test ! -r "$XSLT_CONFIG"; then
        AC_MSG_ERROR([
XSLT required.
])
fi

with_xslt_include=`$XSLT_CONFIG --prefix`
AC_MSG_NOTICE([using XSLT headers at $with_xslt_include])
with_xslt_lib=`$XSLT_CONFIG --prefix`
AC_MSG_NOTICE([using XSLT libraries at $with_xslt_lib])

XSLT_LIB=`$XSLT_CONFIG --libs`
AC_SUBST(XSLT_LIB)
XSLT_INCLUDE=`$XSLT_CONFIG --cflags`
AC_SUBST(XSLT_INCLUDE)


# OpenSSL
OPENSSL_INC_DIR=
AC_ARG_WITH([openssl],
	[AS_HELP_STRING([--with-openssl@<:@=PATH@:>@],
		[Especifica la ruta de instalacion de la libreria OpenSSL @<:@default=/usr@:>@])],
	[
		LIBS="-L$withval/lib ${LIBS}"
		LDFLAGS="${LDFLAGS} -L$withval/lib"
		CFLAGS="${CFLAGS} -I$withval/include"
		OPENSSL_INC_DIR="$withval/include"
	],
	[
		LIBS="-L/usr/lib ${LIBS}"
		LDFLAGS="${LDFLAGS} -L/usr/lib"
		CFLAGS="${CFLAGS} -I/usr/include"
		OPENSSL_INC_DIR="/usr/include"
	])
AC_SUBST(OPENSSL_INC_DIR)
AC_CHECK_HEADERS([openssl/evp.h])
OPENSSL_LIBS=
AC_CHECK_LIB(crypto, X509_get_pubkey, OPENSSL_LIBS="-lcrypto")
AC_SUBST(OPENSSL_LIBS)

# Now capture the input charset
DEFAULT_CHARSET="UTF-8"
if test "$LANG" ; then
    DEFAULT_CHARSET=`echo $LANG | cut -d '.' -f 2 | cut -d '@' -f 1`
    AC_MSG_NOTICE([$DEFAULT_CHARSET character set used for character conversions.])
else
    AC_MSG_NOTICE([Using utf-8 as default character set for conversions.])
fi
AC_DEFINE_UNQUOTED([INERE_LOCAL_CHARSET], ["$DEFAULT_CHARSET"], [Character set to be used for character conversions to UTF-8l])


AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
		 examples/Makefile
		 python/setup.py])
AC_OUTPUT
