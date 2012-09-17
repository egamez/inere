#!/bin/bash
aclocal
autoheader
libtoolize --force
automake --foreign --add-missing
autoconf
./configure && make
