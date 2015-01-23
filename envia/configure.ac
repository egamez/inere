#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([envia], [0.1.0], [egamez@edisson.com.mx])
AC_CONFIG_SRCDIR([src/envia.c])
AC_CONFIG_HEADERS([include/inere/envia/config.h])
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
AC_CHECK_HEADERS([time.h locale.h stdlib.h string.h iconv.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([localtime_r memset setlocale strdup strrchr iconv])

# Checks for extra libraries.

# Fija el host y domain name
ENVIA_FULLHOSTNAME=`hostname -f`
if test -z "$ENVIA_FULLHOSTNAME" ; then
	# Define un nombre para este nodo
	ENVIA_FULLHOSTNAME="localhost.localdomain"
fi
AC_DEFINE_UNQUOTED([ENVIA_HOSTNAME], ["$ENVIA_FULLHOSTNAME"], [Define el nombre de este nodo. Este nombre se utilizara para formar el identificador del mensaje.])


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


AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile])
AC_OUTPUT
