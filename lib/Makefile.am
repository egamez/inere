#
lib_LTLIBRARIES = libinere.la
libinere_la_SOURCES = cantidades.c homonimia.c numerales.c personafisica.c \
		      verificador.c util.c
nobase_dist_include_HEADERS = $(top_srcdir)/include/inere/cantidades.h \
		$(top_srcdir)/include/inere/homonimia.h \
		$(top_srcdir)/include/inere/numerales.h \
		$(top_srcdir)/include/inere/personafisica.h \
		$(top_srcdir)/include/inere/verificador.h \
		$(top_srcdir)/include/inere/util.h
AM_CPPFLAGS = -D_GNU_SOURCE -pedantic -pedantic-errors -Wextra -Wall -W -fPIC -pipe -funsigned-char -ansi -pthread -I$(top_srcdir)/include
libinere_la_LDFLAGS = -version-info 0:4:0
INCLUDES = -I$(top_srcdir)
CLEANFILES = *~
library_includedir = $(includedir)/inere
