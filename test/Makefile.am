SUBDIRS = nls

# Notes about tests:
#  - Add "small" tests and stuff here.
#  - Add "big"   tests and stuff to explicitly test for (fixed) bugs
#    to libexif-testsuite:
#    * We don't want to force people to download dozens of big test images
#      just for one small library. The ones included here are carefully chosen
#      as a basic self-contained regression test set.
#    * Proper testing is done with the "exif" program in libexif-testsuite.
#      And this is just the lib - we don't have the program available
#      here yet.

TESTS = test-mem test-value test-integers test-parse test-tagtable test-sorted \
	test-fuzzer parse-regression.sh

check_PROGRAMS = test-mem test-mnote test-value test-integers test-parse \
	test-tagtable test-sorted test-fuzzer

LDADD = $(top_builddir)/libexif/libexif.la $(LTLIBINTL)

EXTRA_DIST = parse-regression.sh
