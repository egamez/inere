#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([inere], [0.3.0], [egamez@edisson.com.mx])
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

# Fija el host y domain name
ENVIA_FULLHOSTNAME=`hostname -f`
if test -z "$ENVIA_FULLHOSTNAME" ; then
	# Define un nombre para este nodo
	ENVIA_FULLHOSTNAME="localhost.localdomain"
fi
AC_DEFINE_UNQUOTED([ENVIA_HOSTNAME], ["$ENVIA_FULLHOSTNAME"], [Define el nombre de este nodo. Este nombre se utilizara para formar el identificador del mensaje.])

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

# Checks for libraries.
# cURL
# cURL libraries
AC_PATH_PROG(CURL_CONFIG, curl-config)
if test -z "$CURL_CONFIG" || test ! -r "$CURL_CONFIG"; then
	AC_MSG_ERROR([
cURL required.
])
fi
CURL_LIBS=`$CURL_CONFIG --libs`
AC_SUBST(CURL_LIBS)
CURL_INCLUDES=`$CURL_CONFIG --cflags`
AC_SUBST(CURL_INCLUDES)


# Check if we want to use UUID for message identificators
UUID_CONFIG=
UUID_INCLUDES=
UUID_LIBS=
AC_ARG_WITH([uuid],
	[AS_HELP_STRING([--with-uuid@<:@=PATH@:>@],
		[Utiliza UUID como identificadores para los mensajes, especificando la ruta del script uuid-config @<:@default=/usr/bin@:>@])],
        
	[
		if test "x$withval" = "xyes" ; then
			UUID_CONFIG=/usr/bin/uuid-config
		else
			UUID_CONFIG=$withval/uuid-config
		fi
	],
	[
	])
if test -n "$UUID_CONFIG" && test -x "$UUID_CONFIG" ; then
	UUID_INCLUDES=`$UUID_CONFIG --cflags`
	UUID_LIBS="`$UUID_CONFIG --ldflags` -lm `$UUID_CONFIG --libs`"
	AC_DEFINE([ENVIA_UTILIZA_UUID], [1], [Se utilizaran UUID para el identif
icador del mensaje y los delimitadores de las partes del mensaje])

elif test -n "$UUID_CONFIG" && test ! -x "$UUID_CONFIG" ; then
        
	AC_MSG_ERROR([
No se encontró el programa $UUID_CONFIG automatico de configuración de la API para generar los UUID que se utilizarían como identificadores del mensaje.
])

fi

# Se podrá definir por separado el directorio de los headers
AC_ARG_WITH([uuid-include-dir],
	[AS_HELP_STRING([--with-uuid-include-dir@<:@=PATH@:>@],
		[Define la ruta del directorio en donde se encuentran los headers de la API de UUID para construir los identenficadores de mensaje])],
	[
		UUID_INCLUDES=$withval
		AC_DEFINE([ENVIA_UTILIZA_UUID], [1], [Se utilizaran UUID para el identificador del mensaje y los delimitadores de las partes del mensaje])
	]
	[
	])

# Se podrá definir por separado el directorio de las librerias
AC_ARG_WITH([uuid-lib-dir],
	[AS_HELP_STRING([--with-uuid-lib-dir@<:@=PATH@:>@],
		[Define la ruta del directorio en donde se encuentran la libreria de la API de UUID para construir los identenficadores de mensaje])],
	[
		UUID_LIBS="-L$withval"
		AC_DEFINE([ENVIA_UTILIZA_UUID], [1], [Se utilizaran UUID para el identificador del mensaje y los delimitadores de las partes del mensaje])
	]
	[
	])

# Se podrá definir por separado el nombre de la libreria
AC_ARG_WITH([uuid-lib-name],
	[AS_HELP_STRING([--with-uuid-lib-name@<:@=NOMBRE@:>@],
		[Define el nombre (abreviado) de la libreria de implementación de la API de UUID a utilizarse ('ossp-uuid' o 'uuid')])],
	[
		UUID_LIBS="$UUID_LIBS -l$withval"
	]
	[
	])
AC_SUBST(UUID_INCLUDES)
AC_SUBST(UUID_LIBS)


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
                 lib/src/Makefile
                 lib/cfdi/Makefile
                 lib/envia/Makefile
                 src/Makefile
		 examples/Makefile
                 test/Makefile
		 python/setup.py])
AC_OUTPUT
