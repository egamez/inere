#
bin_PROGRAMS = sello certificado cadena comprobante lee

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS) -ldl

certificado_SOURCES = certificado.c
certificado_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
			$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS) -ldl

cadena_SOURCES = cadena.c
cadena_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS) -ldl

comprobante_SOURCES = comprobante.c
comprobante_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS) -ldl

lee_SOURCES = lee.c
lee_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS) -ldl


INCLUDES = -I$(top_srcdir)/include $(XML2_INCLUDE) $(CURL_INCLUDES) $(UUID_INCLUDES)

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
