INCLUDES =              \
        -I$(top_srcdir)

lib_LTLIBRARIES = libexif.la

libexif_la_LDFLAGS = -version-info @LIBEXIF_VERSION_INFO@
libexif_la_SOURCES = exif.c
libexif_la_LIBADD = -lm

libexifincludedir = $(includedir)/libexif
libexifinclude_HEADERS = exif.h

