INCLUDES =			\
	-I$(top_srcdir)		\
	$(GTK_CFLAGS)		\
	$(GDK_PIXBUF_CFLAGS)

bin_PROGRAMS = gexif

gexif_SOURCES =					\
	gexif.c					\
	gexif-main.c gexif-main.h		\
	gexif-thumbnail.c gexif-thumbnail.h
gexif_LDADD =						\
	$(top_builddir)/libexif/libexif.la		\
	$(top_builddir)/libjpeg/libjpeg.la		\
	$(top_builddir)/libexif-gtk/libexif-gtk.la	\
	$(GTK_LIBS)					\
	$(GDK_PIXBUF_LIBS)
