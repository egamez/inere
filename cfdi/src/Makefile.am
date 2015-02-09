#
bin_PROGRAMS = sello
if RI_ENABLED
  bin_PROGRAMS += ri
endif

sello_SOURCES = sello.c
sello_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		-lz -ldl -lm -lrt


ri_SOURCES = ri.c
ri_LDFLAGS = $(top_srcdir)/lib/libinerecfdi.la \
		-lz -ldl -lm -lrt


INCLUDES = -I$(top_srcdir)/include

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
