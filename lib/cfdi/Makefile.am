#

lib_LTLIBRARIES = libinerecfdi.la
libinerecfdi_la_SOURCES = verifica_sello_digital.c certificado.c sello.c

libinerecfdi_la_LDFLAGS = -version-info 0:1:0

nobase_dist_include_HEADERS = $(top_srcdir)/include/inere/cfdi/verifica_sello_digital.h \
			$(top_srcdir)/include/inere/cfdi/certificado.h \
			$(top_srcdir)/include/inere/cfdi/sello.h

INCLUDES = -I$(top_srcdir)/include $(XML2_INCLUDE) $(XSLT_INCLUDE)

AM_CFLAGS = -Wall -Wextra -Winit-self -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
