nlstestscripts = check-nls.sh

TESTS = $(nlstestscripts)

check_SCRIPTS = $(nlstestscripts)

check_PROGRAMS = test-nls print-localedir

CLEANFILES = $(check_SCRIPTS)

test_nls_LDADD = $(top_builddir)/libexif/libexif.la $(INTLLIBS)

%.sh: %.in Makefile
	sed 's|@top_builddir\@|$(top_builddir)|g;s|@localedir\@|$(localedir)|g' < $< > $@
	chmod +x $@
