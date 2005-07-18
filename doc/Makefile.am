########################################################################
# Conditional rules, depending on tool availability
########################################################################

if HAVE_DOXYGEN

DOXYGEN_FILES =
DOXYGEN_STAMPS = $(HTML_APIDOC_DIR).stamp    $(HTML_APIDOC_INTERNALS_DIR).stamp $(DOXYGEN_OUTPUT_DIR).stamp
DOXYGEN_UPLOAD = $(HTML_APIDOC_DIR).uploaded $(HTML_APIDOC_INTERNALS_DIR).uploaded

# FIXME: Depending on the source files would be sufficient...
$(HTML_APIDOC_DIR).stamp: Doxyfile $(top_builddir)/libexif/libexif.la
	doxygen $<
	echo > $@

# FIXME: Depending on the source files would be sufficient...
$(HTML_APIDOC_INTERNALS_DIR).stamp: Doxyfile-internals $(top_builddir)/libexif/libexif.la
	doxygen $<
	echo > $@

$(DOXYGEN_OUTPUT_DIR).stamp: Doxyfile $(top_builddir)/libexif/libexif.la
	doxygen $<
	echo > $@

$(top_builddir)/libexif/libexif.la:
	cd $(top_builddir)/libexif && $(MAKE) libexif.la

$(HTML_APIDOC_DIR).tar.gz: $(DOXYGEN_OUTPUT_DIR).stamp
	(cd $(DOXYGEN_OUTPUT_DIR) && $(AMTAR) chof - $(HTML_APIDOC_DIR) | GZIP=--best gzip -c) > $@

$(HTML_APIDOC_INTERNALS_DIR).tar.gz: $(DOXYGEN_OUTPUT_DIR).stamp
	(cd $(DOXYGEN_OUTPUT_DIR) && $(AMTAR) chof - $(HTML_APIDOC_INTERNALS_DIR) | GZIP=--best gzip -c) > $@

DOC_APIDOC_HTML = api.html
DOC_APIDOC_INTERNALS_HTML = internals.html

install-apidocs-internals: $(HTML_APIDOC_INTERNALS_DIR).stamp
	$(INSTALL) -m 0755 -d $(DESTDIR)$(docdir)/$(DOC_APIDOC_INTERNALS_HTML)
	$(INSTALL) -m 0644 $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_INTERNALS_DIR)/* $(DESTDIR)$(docdir)/$(DOC_APIDOC_INTERNALS_HTML)/

install-apidocs: $(HTML_APIDOC_DIR).stamp
	$(INSTALL) -m 0755 -d $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)
	$(INSTALL) -m 0644 $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_DIR)/* $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)/

uninstall-apidocs-internals:
	rm -f $(DESTDIR)$(docdir)/$(DOC_APIDOC_INTERNALS_HTML)/*
	rmdir $(DESTDIR)$(docdir)/$(DOC_APIDOC_INTERNALS_HTML)

uninstall-apidocs:
	rm -f $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)/*
	rmdir $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)

clean-apidocs:
	rm -rf $(DOXYGEN_OUTPUT_DIR)
	rm -f $(HTML_APIDOC_DIR).tar.gz

$(DOXYGEN_UPLOAD): $(DOXYGEN_STAMPS)
	find $(DOXYGEN_OUTPUT_DIR) \( -type d -exec chmod a+rx,go-w,g+s {} \; \) \
		-or \( -type f -exec chmod a+r {} \; \)
	rsync -avz --progress $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_DIR)/ sf:/home/users/h/hu/hun/libexif/htdocs/api/
	rsync -avz --progress $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_INTERNALS_DIR)/ sf:/home/users/h/hu/hun/libexif/htdocs/internals/
	ssh "sf" "find /home/groups/l/li/libexif/htdocs/internals /home/groups/l/li/libexif/htdocs/api \( -type d -exec chgrp libexif {} \; -exec chmod g+rwxs,a+rx {} \; \) -or \( -type f -exec chgrp libexif {} \; -exec chmod g+rw {} \; \( -name '*.html' -or -name '*.png' -or -name '*.php' -or -name '*.jpg' -or -name '*.css' -name '*.dot' -name '*.map' \) -exec chmod a+r {} \; \)"

endif


########################################################################
# Common part
########################################################################

EXTRA_DIST = Doxyfile.in Doxyfile-internals.in README.apidocs $(HTML_APIDOC_DIR).tar.gz

doc_DATA = $(DOXYGEN_FILES) README.apidocs

all-local: $(DOXYGEN_STAMPS)

install-data-local: install-apidocs install-apidocs-internals

uninstall-local: uninstall-apidocs uninstall-apidocs-internals

clean-local: clean-apidocs

upload: $(DOXYGEN_UPLOAD)

CLEANFILES = $(DOXYGEN_FILES) $(DOXYGEN_STAMPS)

