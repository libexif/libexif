########################################################################
# Conditional rules, depending on tool availability
########################################################################

if HAVE_DOXYGEN

DOXYGEN_FILES =
DOXYGEN_STAMPS = $(DOXYGEN_OUTPUT_DIR).stamp

$(DOXYGEN_OUTPUT_DIR).stamp: Doxyfile $(top_builddir)/libexif/libexif.la
	doxygen Doxyfile
	echo > $@

$(HTML_APIDOC_DIR).tar.gz: $(DOXYGEN_OUTPUT_DIR).stamp
	(cd $(DOXYGEN_OUTPUT_DIR) && $(TAR) chof - $(HTML_APIDOC_DIR) | GZIP=--best gzip -c) > $@

DOC_APIDOC_HTML = apidocs.html

install-apidocs: $(DOXYGEN_OUTPUT_DIR).stamp
	$(INSTALL) -m 0755 -d $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)
	$(INSTALL) -m 0644 $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_DIR)/* $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)/

uninstall-apidocs:
	rm -f $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)/*
	rmdir $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)

clean-apidocs:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

endif


########################################################################
# Common part
########################################################################

EXTRA_DIST = Doxyfile.in README.apidocs $(HTML_APIDOC_DIR).tar.gz

doc_DATA = $(DOXYGEN_FILES) README.apidocs

all-local: $(DOXYGEN_STAMPS)

install-data-local: install-apidocs

uninstall-local: uninstall-apidocs

clean-local: clean-apidocs

CLEANFILES = $(DOXYGEN_FILES) $(DOXYGEN_STAMPS)

