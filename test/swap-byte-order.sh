#!/bin/sh
# Swaps the byte order of test EXIF files and ensures the data don't change.
srcdir="${srcdir:-.}"
TMPLOG="$(mktemp)"
trap 'rm -f "${TMPLOG}"' 0
# Ensure that names are untranslated
LANG=
LANGUAGE=
LC_ALL=C
export LANG LANGUAGE LC_ALL
for fn in "${srcdir}"/testdata/*.jpg ; do
    ./test-parse --swap-byte-order "${fn}" | sed -e '/^New byte order:/d' > "${TMPLOG}"
    if ! diff "${fn}".parsed "${TMPLOG}"; then
        echo Error parsing "$fn"
        exit 1
    fi
done
