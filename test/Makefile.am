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
	test-fuzzer test-null parse-regression.sh swap-byte-order.sh \
	extract-parse.sh

if USE_FAILMALLOC
TESTS += check-failmalloc.sh
endif

check_PROGRAMS = test-mem test-mnote test-value test-integers test-parse \
	test-tagtable test-sorted test-fuzzer test-extract test-null

LDADD = $(top_builddir)/libexif/libexif.la $(LTLIBINTL)

EXTRA_DIST = check-vars.sh.in parse-regression.sh swap-byte-order.sh \
	extract-parse.sh check-failmalloc.sh \
	testdata/canon_makernote_variant_1.jpg \
	testdata/canon_makernote_variant_1.jpg.parsed \
	testdata/fuji_makernote_variant_1.jpg \
	testdata/fuji_makernote_variant_1.jpg.parsed \
	testdata/olympus_makernote_variant_2.jpg \
	testdata/olympus_makernote_variant_2.jpg.parsed \
	testdata/olympus_makernote_variant_3.jpg \
	testdata/olympus_makernote_variant_3.jpg.parsed \
	testdata/olympus_makernote_variant_4.jpg \
	testdata/olympus_makernote_variant_4.jpg.parsed \
	testdata/olympus_makernote_variant_5.jpg \
	testdata/olympus_makernote_variant_5.jpg.parsed \
	testdata/pentax_makernote_variant_2.jpg \
	testdata/pentax_makernote_variant_2.jpg.parsed \
	testdata/pentax_makernote_variant_3.jpg \
	testdata/pentax_makernote_variant_3.jpg.parsed \
	testdata/pentax_makernote_variant_4.jpg \
	testdata/pentax_makernote_variant_4.jpg.parsed
