#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([inere], [0.1.1], [egamez@edisson.com.mx])
AC_CONFIG_SRCDIR([src/rfc.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# For libtool
AC_PROG_LIBTOOL

# For automake
AM_INIT_AUTOMAKE

# Checks for libraries.

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

AC_SUBST(with_xml2_lib)
AC_SUBST(with_xml2_include)
XML2_LIB=`$XML2_CONFIG --libs`
AC_SUBST(XML2_LIB)
XML2_INCLUDE=`$XML2_CONFIG --cflags`
AC_SUBST(XML2_INCLUDE)
XML2_PREFIX=`$XML2_CONFIG --prefix`
AC_SUBST(XML2_PREFIX)

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

AC_SUBST(with_xslt_lib)
AC_SUBST(with_xslt_include)
XSLT_LIB=`$XSLT_CONFIG --libs`
AC_SUBST(XSLT_LIB)
XSLT_INCLUDE=`$XSLT_CONFIG --cflags`
AC_SUBST(XSLT_INCLUDE)


# OpenSSL
AC_DEFINE([EDISSON_OPENSSL_INCDIR], [], [Directory where the OpenSSL headers files are])
EDISSON_OPENSSL_INCDIR="$opensslhdrval"
AC_SUBST(EDISSON_OPENSSL_INCDIR)
AC_MSG_RESULT($opensslhdrval)

opensslhdr=yes
AC_CHECK_HEADER(pem.h ,AC_DEFINE(EDISSON_HAVE_OPENSSL,1,
[Define if you have the <openssl/pem.h> header]),
opensslhdr=no)
OPENSSL_LIBS=
AC_CHECK_LIB(crypto,X509_get_pubkey,OPENSSL_LIBS="-lcrypto $OPENSSL_LIBS")
AC_SUBST(OPENSSL_LIBS)



# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([strsep strcasecmp])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
		 python/setup.py])
AC_OUTPUT
