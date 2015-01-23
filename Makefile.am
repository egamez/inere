#
SUBDIRS = lib src

if ENVIA_ENABLED
  SUBDIRS += envia
endif
if CFDI_ENABLED
  SUBDIRS += cfdi
endif

EXTRA_DIST = autogen.sh \
	autoclean.sh \
	README.md \
	examples/Makefile.in \
	examples/obten_digito.c \
	examples/obten_homonimia.c \
	examples/obten_rfc.c \
	examples/verifica_rfc.c \
	examples/moral.c \
	examples/extrae.c \
	python/inere.c \
	python/setup.py.in \
	js/README.md \
	js/jquery.clave_rfc.js \
	js/jquery.clave_rfc.min.js \
	js/fisica.html \
	js/moral.html \
	js/rfc.html

ACLOCAL_AMFLAGS = -I m4
