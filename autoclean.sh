# Useful script to remove unneeded files when building the entire chain

rm -rf aclocal.m4 \
	autom4te.cache/ \
	config.guess \
	config.sub \
	configure \
	depcomp \
	install-sh \
	ltmain.sh \
	m4/ \
	Makefile.in \
	missing \
	examples/extrae \
	examples/extrae.o \
	examples/moral \
	examples/moral.o \
	examples/obten_digito \
	examples/obten_digito.o \
	examples/obten_homonimia \
	examples/obten_homonimia.o \
	examples/obten_rfc \
	examples/obten_rfc.o \
	examples/verifica_rfc \
	examples/verifica_rfc.o \
	include/inere/config.h.in~ \
	lib/Makefile.in \
	lib/src/Makefile.in \
	lib/cfdi/Makefile.in \
	lib/envia/Makefile.in \
	test/Makefile.in \
	python/build/ \
	src/Makefile.in

# envia
cd envia
./autoclean.sh

# cfdi
cd ../cfdi
./autoclean.sh
