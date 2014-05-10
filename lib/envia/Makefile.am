#
lib_LTLIBRARIES = libinereenvia.la
libinereenvia_la_SOURCES = curl_envia_email.c genera_cadena.c non_ascii.c

libinereenvia_la_LDFLAGS = -version 0:1:0

nobase_dist_include_HEADERS = $(top_srcdir)/include/inere/envia/non_ascii.h \
			$(top_srcdir)/include/inere/envia/curl_envia_email.h \
			$(top_srcdir)/include/inere/envia/genera_cadena.h

INCLUDES = -I$(top_srcdir)/include $(CURL_INCLUDES) $(UUID_INCLUDES)

AM_CFLAGS = -Wall -Wextra -Wuninitialized -Wmissing-declarations -Winit-self -ansi -pedantic -pthread -pedantic-errors -D_GNU_SOURCE -funsigned-char
