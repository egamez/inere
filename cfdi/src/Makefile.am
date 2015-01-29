#
bin_PROGRAMS = sello
if RI_ENABLED
  bin_PROGRAMS += ri
endif

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS)

ri_SOURCES = ri.c
ri_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		$(XML2_LIB) $(XSLT_LIB) $(OPENSSL_LIBS) $(LIBQRENCODE_LIBS) \
		$(LIBHPDF_LIBS) $(LIBPNG_LIBS)

INCLUDES = -I$(top_srcdir)/include $(XML2_INCLUDE) $(LIBPNG_INCLUDES) \
	   $(LIBQRENCODE_INCLUDES) $(LIBHPDF_INCLUDES)

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
