#!/bin/sh
# Run this to generate all the initial makefiles, etc.
# This was lifted from the Gimp, and adapted slightly by
# Raph Levien .

DIE=0
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
test "$srcdir" = "." && srcdir=`pwd`

PROJECT=libexif

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have autoconf installed to compile $PROJECT."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
    DIE=1
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have libtool installed to compile $PROJECT."
    echo "Get ftp://ftp.gnu.org/pub/gnu/libtool-1.2.tar.gz"
    echo "(or a newer version if it is available)"
    DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have automake installed to compile $PROJECT."
    echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.3.tar.gz"
    echo "(or a newer version if it is available)"
    DIE=1
}

if test "$DIE" -eq 1; then
    exit 1
fi

test -f libexif/exif.h || {
        echo "You must run this script in the top-level gphoto2 directory"
        exit 1
}

if test -z "$*"; then
    echo "I am going to run ./configure with no arguments - if you wish "
    echo "to pass any to it, please specify them on the $0 command line."
fi


case "$CC" in
*xlc | *xlc\ * | *lcc | *lcc\ *) am_opt=--include-deps;;
esac

libtoolize
gettextize
echo "Running aclocal${AUTOMAKE_SUFFIX} $ACLOCAL_FLAGS"
aclocal${AUTOMAKE_SUFFIX} $ACLOCAL_FLAGS
echo "Running autoheader"
autoheader
echo "Running automake${AUTOMAKE_SUFFIX} --add-missing --gnu $am_opt"
automake${AUTOMAKE_SUFFIX} --add-missing --gnu $am_opt
echo "Running autoconf"
autoconf
echo "Running ./configure"
./configure "$@"

echo 
echo "Now type 'make' to compile $PROJECT."
