#! /bin/bash
aclocal -I . -I m4 --force
autoheader -f
libtoolize --copy --force
automake --add-missing --copy --foreign
autoconf --force
