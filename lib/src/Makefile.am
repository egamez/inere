#

lib_LTLIBRARIES = libinere.la
libinere_la_SOURCES = cantidadcl_alloc.c homonimia.c numeral_alloc.c \
		      personafisica.c personamoral.c verificador.c util.c \
		      extrae_fecha.c base64.c

libinere_la_LDFLAGS = -version-info 1:2:0

nobase_dist_include_HEADERS = $(top_srcdir)/include/inere/cantidadcl_alloc.h \
		$(top_srcdir)/include/inere/homonimia.h \
		$(top_srcdir)/include/inere/numeral_alloc.h \
		$(top_srcdir)/include/inere/personafisica.h \
		$(top_srcdir)/include/inere/personamoral.h \
		$(top_srcdir)/include/inere/verificador.h \
		$(top_srcdir)/include/inere/util.h \
		$(top_srcdir)/include/inere/extrae_fecha.h \
		$(top_srcdir)/include/inere/base64.h

INCLUDES = -I$(top_srcdir)/include

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
