nlstestscripts = check-localedir.sh

TESTS = $(nlstestscripts)

check_SCRIPTS = $(nlstestscripts)

check_PROGRAMS = print-localedir

CLEANFILES = $(check_SCRIPTS)

# test_nls_LDADD = $(top_builddir)/libexif/libexif.la $(INTLLIBS)

EXTRA_DIST = check-localedir.in check-nls.in test-nls.c

%.sh: %.in Makefile
	sed 's|@top_builddir\@|$(top_builddir)|g;s|@localedir\@|$(localedir)|g;s|@PRINT_LOCALEDIR\@|./print-localedir$(EXEEXT)|g;s|@build_alias\@|$(build_alias)|g;s|@host_alias\@|$(host_alias)|g' < $< > $@
	chmod +x $@
