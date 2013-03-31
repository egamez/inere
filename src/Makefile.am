#
bin_PROGRAMS = cantidad rfc sello

cantidad_SOURCES = cantidad.c
cantidad_LDFLAGS = $(top_srcdir)/lib/libinere.la

rfc_SOURCES = rfc.c
rfc_LDFLAGS = $(top_srcdir)/lib/libinere.la

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/libinere.la $(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS)

CLEANFILES = *~

AM_CPPFLAGS = -D_GNU_SOURCE -pedantic -pedantic-errors -Wextra -Wall -W -fPIC -pipe -funsigned-char -ansi -pthread -I$(top_srcdir)/include $(XML2_INCLUDE)
