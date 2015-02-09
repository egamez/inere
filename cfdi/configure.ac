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
AC_ARG_WITH([xml2-config],
	[AS_HELP_STRING([--with-xml2-config@<:@=PATH@:>@],
		[Especifica la ruta del script xml2-config @<:@default=/usr/local/bin@:>@])],
	[
		if test "x${withval}" = "xyes" ; then
			XML2_CONFIG=/usr/local/bin/xml2-config
		else
			XML2_CONFIG=${withval}/xml2-config
		fi
	],
	[
		AC_PATH_PROGS(XML2_CONFIG, xml2-config)
	])
if test -z "${XML2_CONFIG}" || test ! -r "${XML2_CONFIG}"; then
	AC_MSG_ERROR([
libXML2 (through xml2-config) required.
])
fi



# Verifica la instalacion
CFLAGS="`${XML2_CONFIG} --cflags` ${CFLAGS}"
LIBS="-L`${XML2_CONFIG} --prefix`/lib ${LIBS}"
AC_CHECK_HEADERS([libxml/tree.h])
AC_SEARCH_LIBS([xmlParseFile],
		[xml2],
		[],
		[
			AC_MSG_NOTICE([No se encontro la libreria libxml2, necesaria para compilar soporte para lectura y escritura de CFDIs])
		],
		[-lz -lm -ldl])



# xslt
XSLT_CONFIG=
AC_ARG_WITH([xslt-config],
	[AS_HELP_STRING([--with-xslt-config@<:@=PATH@:>@],
		[Especifica la ruta del script xslt-config @<:@default=/usr/local/bin@:>@])],
	[
		if test "x${withval}" = "xyes" ; then
			XSLT_CONFIG=/usr/local/bin/xslt-config
		else
			XSLT_CONFIG=${withval}/xslt-config
		fi
	],
	[
		AC_PATH_PROGS(XSLT_CONFIG, xslt-config)
	])
AC_PATH_PROG(XSLT_CONFIG, xslt-config)
if test -z "$XSLT_CONFIG" || test ! -r "$XSLT_CONFIG"; then
	AC_MSG_ERROR([
XSLT required.
])
fi

# Verifica la instalacion
CFLAGS="`${XSLT_CONFIG} --cflags` ${CFLAGS}"
LIBS="-L`${XSLT_CONFIG} --prefix`/lib ${LIBS}"
AC_CHECK_HEADERS([libxslt/transform.h])
AC_SEARCH_LIBS([xsltParseStylesheetFile],
		[xslt],
		[],
		[
			AC_MSG_NOTICE([No se encontro la libreria libxslt, necesaria para compilar soporte para generar la cadena original])
		],
		[-lxml2 -lz -ldl -lm -lrt])



# OpenSSL
AC_ARG_WITH([openssl],
	[AS_HELP_STRING([--with-openssl@<:@=PATH@:>@],
		[Especifica la ruta de instalacion de la libreria OpenSSL @<:@default=/usr@:>@])],
	[
		if test "x$withval" = "xyes" ; then
			LIBS="-L/usr/lib ${LIBS}"
			LDFLAGS="${LDFLAGS} -L/usr/lib"
			CFLAGS="${CFLAGS} -I/usr/include"
		else
			LIBS="-L$withval/lib ${LIBS}"
			LDFLAGS="${LDFLAGS} -L$withval/lib"
			CFLAGS="${CFLAGS} -I$withval/include"
		fi
	],
	[
		LIBS="-L/usr/lib ${LIBS}"
		LDFLAGS="${LDFLAGS} -L/usr/lib"
		CFLAGS="${CFLAGS} -I/usr/include"
	])
AC_CHECK_HEADERS([openssl/evp.h])
AC_CHECK_LIB([crypto], [X509_get_pubkey])


# Representacion Impresa
RI_ENABLED=false
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
AC_ARG_WITH([libpng-config],
	[AS_HELP_STRING([--with-libpng-config@<:@=PATH@:>@],
		[Define la ruta en donde se encuentra el script libpng-config @<:@default=/usr/bin@:>@])],
	[
		RI_ENABLED=true

		if test "x$withval" = "xyes" ; then
			CFLAGS="`/usr/bin/libpng-config --cflags` ${CFLAGS}"
			LIBS="`/usr/bin/libpng-config --L_opts` ${LIBS}"
		else
			CFLAGS="`${withval}/libpng-config --cflags` ${CFLAGS}"
			LIBS="`${withval}/libpng-config --L_opts` ${LIBS}"
		fi
		AC_CHECK_HEADERS([png.h])
		AC_SEARCH_LIBS([png_create_info_struct],
				[png12],
				[],
				[
					AC_MSG_NOTICE([No se encontro la libraria libpng, necesaria para compilar el soporte para la representacion impresa de CFDIs])
					RI_ENABLED=false
				])
	],
	[
		RI_ENABLED=false
	])


# libqrencode
AC_ARG_WITH([libqrencode-dir],
	[AS_HELP_STRING([--with-libqrencode-dir@<:@=PATH@:>@],
		[Define la ruta de instalacion de libqrencode @<:@default=/usr/local@:>@])],
	[
		RI_ENABLED=true

		if test "x$withval" = "xyes" ; then
			LIBS="-L/usr/local/lib ${LIBS}"
			CFLAGS="-I/usr/local/include ${CFLAGS}"
		else
			LIBS="-L${withval}/lib ${LIBS}"
			CFLAGS="-I${withval}/include ${CFLAGS}"
		fi
		AC_CHECK_HEADERS([qrencode.h])
		AC_SEARCH_LIBS([QRcode_encodeString8bit],
				[qrencode],
				[],
				[
					AC_MSG_NOTICE([No se encontro la libraria libqrencode, necesaria para compilar el soporte para la representacion impresa de CFDIs])
					RI_ENABLED=false
				])
	],
	[
		RI_ENABLED=false
	])


# libHpdf
AC_ARG_WITH([libhpdf-dir],
	[AS_HELP_STRING([--with-libhpdf-dir@<:@=PATH@:>@],
		[Define la ruta de instalacion de libhpdf @<:@default=/usr/local@:>@])],
	[
		RI_ENABLED=true

		if test "x$withval" = "xyes" ; then
	 		LIBS="-L/usr/local/lib ${LIBS}"
			CFLAGS="-I/usr/local/include ${CFLAGS}"
		else
			LIBS="-L${withval}/lib ${LIBS}"
			CFLAGS="-I${withval}/include ${CFLAGS}"
		fi
		AC_CHECK_HEADERS([hpdf.h])
		AC_SEARCH_LIBS([HPDF_New],
				[hpdf],
				[],
				[
					AC_MSG_NOTICE([No se encontro la libraria libqrencode, necesaria para compilar el soporte para la representacion impresa de CFDIs])
					RI_ENABLED=false
				])
	],
	[
		RI_ENABLED=false
	])


# Ahora define la ruta de localizacion de las fonts que se utilizaran
# para la generacion de la representacion impresa
AC_ARG_WITH([ttf-font-file],
	[AS_HELP_STRING([--with-ttf-font-file@<:@=PATH@:>@],
		[Define la ruta en donde se localizan las TTF que libHaru utilizara para general la representacion impresa])
	],
	[
		# Ahora verifica que exista un archivo asi
		if test -f ${withval} ; then
			AC_DEFINE_UNQUOTED([INERE_TTF_FONT_PATH], ["${withval}"], [Ruta de las TTF para la representacion impresa])
		else
			AC_MSG_ERROR([El archivo dado como fuente para las TTF, para la representacion impresa, no existe o no es de lectura.])
		fi
	],
	[
		if test "${RI_ENABLED}" = "true" ; then
			AC_MSG_ERROR([Es necesario definir la ruta de las TTF que se utilizaran para la representacion impresa.])
		fi
	])


# TTF BOLD
AC_ARG_WITH([ttf-bold-font-file],
	[AS_HELP_STRING([--with-ttf-bold-font-file@<:@=PATH@:>@],
		[Define la ruta en donde se localizan las TTF, en negritas, que libHaru utilizara para general la representacion impresa])
	],
	[
		# Ahora verifica que exista un archivo asi
		if test -f ${withval} ; then
			AC_DEFINE_UNQUOTED([INERE_TTF_FONT_BOLD_PATH], ["${withval}"], [Ruta de las TTF, en negritas, para la representacion impresa])
		else
			AC_MSG_ERROR([El archivo dado como fuente para las TTF en negritas, para la representacion impresa, no existe o no es de lectura.])
		fi
	],
	[
		if test "${RI_ENABLED}" = "true" ; then
			AC_MSG_ERROR([Es necesario definir la ruta de las TTF, en negritas, que se utilizaran para la representacion impresa.])
		fi
	])


AM_CONDITIONAL([RI_ENABLED], [ test x$RI_ENABLED = xtrue ])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
                 test/Makefile
		 python/setup.py])
AC_OUTPUT
