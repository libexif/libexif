#!/bin/sh
# Parses test EXIF files and compares the results to that expected
srcdir="${srcdir:-.}"
TMPLOG="$(mktemp)"
trap 'rm -f "${TMPLOG}" && rm -f dump.json dump.xml' 0
# Ensure that names are untranslated
LANG=
LANGUAGE=
LC_ALL=C
export LANG LANGUAGE LC_ALL
for fn in "${srcdir}"/testdata/*.jpg ; do
    rm -f dump.json dump.xml
    ./test-parse "${fn}" > "${TMPLOG}"
    if ! diff -b "${fn}".parsed "${TMPLOG}"; then
        echo Error parsing "$fn"
        exit 1
    fi
    if [ ! -f dump.json ]; then
        echo "Could not find dump.json. It is needed to check integrity of json dump action"
        exit 1
    fi
    if [ ! -f dump.xml ]; then
        echo "Could not find dump.xml. It is needed to check integrity of xml dump action"
        exit 1
    fi
    if ! diff -b dump.json "${fn}.json"; then
        echo "json dump produced and recorded is different"
        echo "${fn}.json should be:"
        cat dump.json
        exit 1
    fi

    if ! diff -b dump.xml "${fn}.xml"; then
        echo "xml dump produced and recorded is different"
        echo "${fn}.xml should be:"
        cat dump.xml
        exit 1
    fi
    rm -f dump.json dump.xml
done
