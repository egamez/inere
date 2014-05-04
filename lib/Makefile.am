#

lib_LTLIBRARIES = libinere.la
libinere_la_SOURCES = cantidades.c homonimia.c numerales.c personafisica.c \
		      personamoral.c verificador.c util.c \
		      verifica_sello_digital.c extrae_fecha.c

libinere_la_LDFLAGS = -version-info 1:1:0

nobase_dist_include_HEADERS = $(top_srcdir)/include/inere/cantidades.h \
		$(top_srcdir)/include/inere/homonimia.h \
		$(top_srcdir)/include/inere/numerales.h \
		$(top_srcdir)/include/inere/personafisica.h \
		$(top_srcdir)/include/inere/personamoral.h \
		$(top_srcdir)/include/inere/verificador.h \
		$(top_srcdir)/include/inere/verifica_sello_digital.h \
		$(top_srcdir)/include/inere/util.h \
		$(top_srcdir)/include/inere/extrae_fecha.h

#INCLUDES = -I$(top_srcdir) $(XML2_INCLUDE) $(XSLT_INCLUDE)
#AM_CPPFLAGS = -D_GNU_SOURCE -pedantic -pedantic-errors -Wall -W -fPIC -pipe -funsigned-char -ansi -pthread -I$(top_srcdir)/include $(INCLUDES)
#CLEANFILES = *~
#library_includedir = $(includedir)/inere
INCLUDES = -I$(top_srcdir)/include $(XML2_INCLUDE) $(XSLT_INCLUDE)

AM_CFLAGS = -Wall -Wextra -Winit-self -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char

