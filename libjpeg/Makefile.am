INCLUDES =				\
	-I$(top_srcdir)/libexif		\
	-I$(top_srcdir)intl		\
	-I$(top_srcdir)

noinst_LTLIBRARIES = libjpeg.la
libjpeg_la_SOURCES =			\
	jpeg-data.c jpeg-data.h		\
	jpeg-marker.c jpeg-marker.h
libjpeg_la_LIBADD = ../libexif/libexif.la
