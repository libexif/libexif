########################################################################
# Conditional rules, depending on tool availability
########################################################################

if HAVE_DOXYGEN

DOXYGEN_FILES =
DOXYGEN_STAMPS = $(DOXYGEN_OUTPUT_DIR).stamp
DOXYGEN_UPLOAD = $(DOXYGEN_OUTPUT_DIR).uploaded

# FIXME: Depending on the source files would be sufficient...
$(DOXYGEN_OUTPUT_DIR).stamp: Doxyfile $(top_builddir)/libexif/libexif.la
	doxygen Doxyfile
	echo > $@

$(top_builddir)/libexif/libexif.la:
	cd $(top_builddir)/libexif && $(MAKE) libexif.la

$(HTML_APIDOC_DIR).tar.gz: $(DOXYGEN_OUTPUT_DIR).stamp
	(cd $(DOXYGEN_OUTPUT_DIR) && $(AMTAR) chof - $(HTML_APIDOC_DIR) | GZIP=--best gzip -c) > $@

DOC_APIDOC_HTML = apidocs.html

install-apidocs: $(DOXYGEN_OUTPUT_DIR).stamp
	$(INSTALL) -m 0755 -d $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)
	$(INSTALL) -m 0644 $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_DIR)/* $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)/

uninstall-apidocs:
	rm -f $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)/*
	rmdir $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)

clean-apidocs:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

$(DOXYGEN_UPLOAD): $(DOXYGEN_STAMPS)
	find $(DOXYGEN_OUTPUT_DIR) \( -type d -exec chmod a+rx,go-w,g+s {} \; \) \
		-or \( -type f -exec chmod a+r {} \; \)
	rsync -avz --progress $(DOXYGEN_OUTPUT_DIR)/libexif-api.html/ sf:/home/users/h/hu/hun/libexif/htdocs/api/
	ssh "sf" "find /home/groups/l/li/libexif/htdocs/api \( -type d -exec chgrp libexif {} \; -exec chmod g+rwxs,a+rx {} \; \) -or \( -type f -exec chgrp libexif {} \; -exec chmod g+rw {} \; \( -name '*.html' -or -name '*.png' -or -name '*.php' -or -name '*.jpg' -or -name '*.css' -name '*.dot' -name '*.map' \) -exec chmod a+r {} \; \)"

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

upload: $(DOXYGEN_UPLOAD)

CLEANFILES = $(DOXYGEN_FILES) $(DOXYGEN_STAMPS)

