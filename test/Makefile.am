SUBDIRS = nls

noinst_PROGRAMS = test-mem test-tree test-mnote test-value

test_tree_LDADD = $(top_builddir)/libjpeg/libjpeg.la $(top_builddir)/libexif/libexif.la $(INTLLIBS)
test_mem_LDADD   = $(top_builddir)/libexif/libexif.la $(INTLLIBS)
test_mnote_LDADD = $(top_builddir)/libexif/libexif.la $(INTLLIBS)
test_value_LDADD = $(top_builddir)/libexif/libexif.la $(INTLLIBS)
