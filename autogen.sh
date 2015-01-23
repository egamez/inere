#!/bin/bash
aclocal
autoheader
libtoolize --force
automake --foreign --add-missing
autoconf

# envia
cd envia
aclocal
autoheader
libtoolize --force
automake --foreign --add-missing
autoconf

# cfdi
cd ../cfdi
aclocal
autoheader
libtoolize --force
automake --foreign --add-missing
autoconf
