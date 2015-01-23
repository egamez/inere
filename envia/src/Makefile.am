#
bin_PROGRAMS = envia

envia_SOURCES = envia.c
envia_LDFLAGS = $(top_srcdir)/lib/libinereenvia.la \
                $(CURL_LIBS) $(UUID_LIBS)

INCLUDES = -I$(top_srcdir)/include $(CURL_INCLUDES) $(UUID_INCLUDES)

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
