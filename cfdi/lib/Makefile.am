#

lib_LTLIBRARIES = libinerecfdi.la
libinerecfdi_la_SOURCES = verifica_sello_digital.c lee_certificado.c sello.c \
			  genera_cadena_original.c crea_cfdi.c lee_cfdi.c \
			  base64.c

if RI_ENABLED
  libinerecfdi_la_SOURCES += crea_cfdi_qrcode_alloc.c r12nimpresa.c
endif

libinerecfdi_la_LDFLAGS = -version-info 0:1:0

nobase_dist_include_HEADERS = $(top_srcdir)/include/inere/cfdi/verifica_sello_digital.h \
			$(top_srcdir)/include/inere/cfdi/lee_certificado.h \
			$(top_srcdir)/include/inere/cfdi/genera_cadena_original.h \
			$(top_srcdir)/include/inere/cfdi/sello.h \
			$(top_srcdir)/include/inere/cfdi/crea_cfdi.h \
			$(top_srcdir)/include/inere/cfdi/comprobante.h \
			$(top_srcdir)/include/inere/cfdi/lee_cfdi.h \
			$(top_srcdir)/include/inere/cfdi/base64.h

if RI_ENABLED
  nobase_dist_include_HEADERS += $(top_srcdir)/include/inere/cfdi/crea_cfdi_qrcode_alloc.h \
				 $(top_srcdir)/include/inere/cfdi/r12nimpresa.h
endif

AM_CPPFLAGS = -I$(top_srcdir)/include
if RI_ENABLED
  AM_CPPFLAGS += -I$(top_srcdir)/../include
endif

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
