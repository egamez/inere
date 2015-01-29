#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([cfdi], [0.1.0], [egamez@edisson.com.mx])
AC_CONFIG_SRCDIR([src/sello.c])
AC_CONFIG_HEADERS([include/inere/cfdi/config.h])
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
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([localtime_r memset])

# Checks for extra libraries.

# Check for libxml2
XML2_CONFIG=
AC_ARG_WITH([xml2config],
	[AS_HELP_STRING([--with-xml2config@<:@=PATH@:>@],
		[Especifica la ruta del ejecutable xml2-config @<:@default=/usr/local/bin@:>@])],
	[
		XML2_CONFIG=$withval/xml2-config
	],
	[
		AC_PATH_PROGS(XML2_CONFIG, xml2-config)
	])
if test -z "$XML2_CONFIG" || test ! -r "$XML2_CONFIG"; then
	AC_MSG_ERROR([
libXML2 (through xml2-config) required.
])
fi

with_xml2_include=`$XML2_CONFIG --prefix`
AC_MSG_NOTICE([using libxml2 headers at $with_xml2_include])
with_xml2_lib=`$XML2_CONFIG --prefix`
AC_MSG_NOTICE([using libxml2 libraries at $with_xml2_lib])

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
		if test "x$withval" = "xyes" ; then
			LIBS="-L/usr/lib ${LIBS}"
			LDFLAGS="${LDFLAGS} -L/usr/lib"
			CFLAGS="${CFLAGS} -I/usr/include"
			OPENSSL_INC_DIR="/usr/include"
		else
			LIBS="-L$withval/lib ${LIBS}"
			LDFLAGS="${LDFLAGS} -L$withval/lib"
			CFLAGS="${CFLAGS} -I$withval/include"
			OPENSSL_INC_DIR="$withval/include"
		fi
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


# Representacion Impresa
RI_ENABLED=no
AC_ARG_ENABLE([ri],
	[AS_HELP_STRING([--enable-ri],
		[Compila programas para soportar la representacion impresa de CFDIs])],
	[
		case "${enableval}" in
			yes)	RI_ENABLED=true  ;;
			no)	RI_ENABLED=false ;;
			*)	AC_MSG_ERROR([valor inapropiado ${enableval} para --enable-ri]) ;;
		esac
	],
	[
		RI_ENABLED=false
	])


# libpng
LIBPNG_LIBS=
LIBPNG_INCLUDES=
AC_ARG_WITH([libpng-config],
	[AS_HELP_STRING([--with-libpng-config@<:@=PATH@:>@],
		[Define la ruta en donde se encuentra el script libpng-config @<:@default=/usr/bin@:>@])],
	[
		RI_ENABLED=true

		if test "x$withval" = "xyes" ; then
			LIBPNG_LIBS="-L`/usr/bin/libpng-config --libdir` `/usr/bin/libpng-config --libs`"
			LIBPNG_INCLUDES=`/usr/bin/libpng-config --cflags`
		else
			LIBPNG_LIBS="-L`${withval}/libpng-config --libdir` `${withval}/libpng-config --libs`"
			LIBPNG_INCLUDES=`${withval}/libpng-config --cflags`
		fi
	],
	[
		RI_ENABLED=false
		AC_MSG_NOTICE([libpng not selected?])
	])
AC_SUBST(LIBPNG_LIBS)
AC_SUBST(LIBPNG_INCLUDES)


# libqrencode
LIBQRENCODE_INCLUDES=
LIBQRENCODE_LIBS=
AC_ARG_WITH([libqrencode-dir],
	[AS_HELP_STRING([--with-libqrencode-dir@<:@=PATH@:>@],
		[Define la ruta de instalacion de libqrencode @<:@default=/usr/local@:>@])],
	[
		RI_ENABLED=true

		if test "x$withval" = "xyes" ; then
			LIBQRENCODE_LIBS="-L/usr/local/lib -lqrencode"
			LIBQRENCODE_INCLUDES="-I/usr/local/include"
		else
			LIBQRENCODE_LIBS="-L$withval/lib -lqrencode"
			LIBQRENCODE_INCLUDES="-I$withval/include"
		fi
	],
	[
		RI_ENABLED=false
	])
AC_SUBST(LIBQRENCODE_LIBS)
AC_SUBST(LIBQRENCODE_INCLUDES)


# libHpdf
LIBHPDF_INCLUDES=
LIBHPDF_LIBS=
AC_ARG_WITH([libhpdf-dir],
	[AS_HELP_STRING([--with-libhpdf-dir@<:@=PATH@:>@],
		[Define la ruta de instalacion de libhpdf @<:@default=/usr/local@:>@])],
	[
		RI_ENABLED=true

		if test "x$withval" = "xyes" ; then
			LIBHPDF_LIBS="-L/usr/local/lib -lhpdf"
			LIBHPDF_INCLUDES="-I/usr/local/include"
		else
			LIBHPDF_LIBS="-L$withval/lib -lhpdf"
			LIBHPDF_INCLUDES="-I$withval/include"
		fi
	],
	[
		RI_ENABLED=false
	])
AC_SUBST(LIBHPDF_LIBS)
AC_SUBST(LIBHPDF_INCLUDES)


AM_CONDITIONAL([RI_ENABLED], [ test x$RI_ENABLED = xtrue ])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
                 test/Makefile
		 python/setup.py])
AC_OUTPUT
