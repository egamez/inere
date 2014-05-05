#
bin_PROGRAMS = cantidad rfc sello envia

cantidad_SOURCES = cantidad.c
cantidad_LDFLAGS = $(top_srcdir)/lib/src/libinere.la

rfc_SOURCES = rfc.c
rfc_LDFLAGS = $(top_srcdir)/lib/src/libinere.la

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/cfdi/libinerecfdi.la \
		$(top_srcdir)/lib/src/libinere.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS)

envia_SOURCES = envia.c
envia_LDFLAGS = $(top_srcdir)/lib/envia/libinereenvia.la \
		$(top_srcdir)/lib/src/libinere.la \
                $(CURL_LIBS) $(UUID_LIBS)

INCLUDES = -I$(top_srcdir)/include $(XML2_INCLUDE) $(CURL_INCLUDES) $(UUID_INCLUDES)

AM_CFLAGS = -Wall -Wextra -Winit-self -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
