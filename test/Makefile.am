INCLUDES =				\
	-I$(top_srcdir)/libexif		\
	-I$(top_srcdir)intl		\
	-I$(top_srcdir)	

noinst_PROGRAMS =	\
	test-mem	\
	test-tree

test_tree_LDADD =		\
	../libjpeg/libjpeg.la	\
	../libexif/libexif.la	\
	$(INTLLIBS)

test_mem_LDADD =		\
	../libexif/libexif.la	\
	$(INTLLIBS)
