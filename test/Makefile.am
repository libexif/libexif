INCLUDES =				\
	-I$(top_srcdir)/libexif		\
	-I$(top_srcdir)intl		\
	-I$(top_srcdir)	

noinst_PROGRAMS = test-mem test-tree test-mnote

test_tree_LDADD = $(top_builddir)/libjpeg/libjpeg.la $(top_builddir)/libexif/libexif.la $(INTLLIBS)
test_mem_LDADD = $(top_builddir)/libexif/libexif.la $(INTLLIBS)
test_mnote_LDADD = $(top_builddir)/libexif/libexif.la $(INTLLIBS)
