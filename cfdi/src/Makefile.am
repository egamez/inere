#
bin_PROGRAMS = sello

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS)

INCLUDES = -I$(top_srcdir)/include $(XML2_INCLUDE)

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
