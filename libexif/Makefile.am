SUBDIRS = canon fuji hp olympus

INCLUDES =              				\
	-DLIBEXIF_LOCALEDIR=\""$(datadir)/locale"\"	\
        -I$(top_srcdir)

lib_LTLIBRARIES = libexif.la

libexif_la_LDFLAGS = -version-info $(LIBEXIF_CURRENT):$(LIBEXIF_REVISION):$(LIBEXIF_AGE)
libexif_la_SOURCES =		\
	exif-byte-order.c	\
	exif-content.c		\
	exif-data.c		\
	exif-entry.c		\
	exif-format.c		\
	exif-i18n.h		\
	exif-note.c		\
	exif-tag.c		\
	exif-utils.c
libexif_la_LIBADD = -lm			\
	canon/libexif-canon.la		\
	fuji/libexif-fuji.la		\
	hp/libexif-hp.la	\
	olympus/libexif-olympus.la

libexifincludedir = $(includedir)/libexif
libexifinclude_HEADERS = 	\
	exif-byte-order.h	\
	exif-content.h		\
	exif-data.h		\
	exif-entry.h		\
	exif-format.h		\
	exif-note.h		\
	exif-result.h		\
	exif-tag.h		\
	exif-utils.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libexif.pc
EXTRA_DIST = libexif.pc.in

