INCLUDES =			\
	-I$(top_srcdir)		\
	$(GTK_CFLAGS)
	

if HAVE_GTK
PROGRAMS_GTK =		\
	test-browser
endif

noinst_PROGRAMS =	\
	$(PROGRAMS_GTK)	\
	test-tree

test_tree_LDADD =		\
	../libjpeg/libjpeg.la	\
	../libexif/libexif.la
test_browser_LDADD = 		\
	../libjpeg/libjpeg.la	\
	../libexif-gtk/libexif-gtk.la
