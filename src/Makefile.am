#
bin_PROGRAMS = cantidad rfc

cantidad_SOURCES = cantidad.c
cantidad_LDFLAGS = $(top_srcdir)/lib/libinere.la

rfc_SOURCES = rfc.c
rfc_LDFLAGS = $(top_srcdir)/lib/libinere.la

CLEANFILES = *~

AM_CPPFLAGS = -pedantic -pedantic-errors -Wextra -Wall -W -fPIC -pipe -funsigned-char -ansi -pthread -I$(top_srcdir)/include
