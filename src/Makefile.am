#
bin_PROGRAMS = cantidad rfc sello

cantidad_SOURCES = cantidad.c
cantidad_LDFLAGS = $(top_srcdir)/lib/libinere.la $(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS)

rfc_SOURCES = rfc.c
rfc_LDFLAGS = $(top_srcdir)/lib/libinere.la $(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS)

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/libinere.la $(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS)

INCLUDES = -I$(top_srcdir)/include $(XML2_INCLUDE)

AM_CFLAGS = -Wall -Wextra -Winit-self -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
