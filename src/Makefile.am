#
#bin_PROGRAMS = cantidad rfc sello envia
bin_PROGRAMS = cantidad rfc

cantidad_SOURCES = cantidad.c
cantidad_LDFLAGS = $(top_srcdir)/lib/libinere.la

rfc_SOURCES = rfc.c
rfc_LDFLAGS = $(top_srcdir)/lib/libinere.la

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
