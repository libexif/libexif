INCLUDES =              				\
	-DLIBEXIF_LOCALEDIR=\""$(datadir)/locale"\"	\
	-I$(top_srcdir)/libexif				\
	-I$(top_srcdir)/libjpeg				\
	-I$(top_srcdir)/intl				\
	-I$(top_srcdir)

lib_LTLIBRARIES = libexif.la

libexif_la_LDFLAGS = -version-info @LIBEXIF_VERSION_INFO@
libexif_la_SOURCES =		\
	exif-byte-order.c	\
	exif-content.c		\
	exif-data.c		\
	exif-entry.c		\
	exif-format.c		\
	exif-ifd.c		\
	exif-tag.c		\
	exif-utils.c		\
	i18n.h
libexif_la_LIBADD = -lm			\
	canon/libexif-canon.la		\
	fuji/libexif-fuji.la		\
	hp/libexif-hp.la		\
	olympus/libexif-olympus.la

libexifincludedir = $(includedir)/libexif
libexifinclude_HEADERS = 	\
	exif-byte-order.h	\
	exif-content.h		\
	exif-data.h		\
	exif-entry.h		\
	exif-format.h		\
	exif-ifd.h		\
	exif-note.h		\
	exif-result.h		\
	exif-tag.h		\
	exif-utils.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libexif.pc
EXTRA_DIST = libexif.pc.in

