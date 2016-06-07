#
bin_PROGRAMS = sello certificado cadena lee crea verifica_firma

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(top_srcdir)/../lib/libinere.la \
		-lz -ldl -lm -lrt

certificado_SOURCES = certificado.c
certificado_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(top_srcdir)/../lib/libinere.la \
		-lz -ldl -lm -lrt

cadena_SOURCES = cadena.c
cadena_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(top_srcdir)/../lib/libinere.la \
		-lz -ldl -lm -lrt

lee_SOURCES = lee.c
lee_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(top_srcdir)/../lib/libinere.la \
		-lz -ldl -lm -lrt

crea_SOURCES = crea.c
crea_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(top_srcdir)/../lib/libinere.la \
		-lz -ldl -lm -lrt

verifica_firma_SOURCES = verifica_firma.c
verifica_firma_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(top_srcdir)/../lib/libinere.la \
		-lz -ldl -lm -lrt


AM_CPPFLAGS = -I$(top_srcdir)/include

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
