########################################################################
# Conditional rules, depending on tool availability
########################################################################

DOXYGEN_FILES =
DOXYGEN_STAMPS =
DOXYGEN_UPLOAD =

DOXYGEN_INSTALL_TARGETS =
DOXYGEN_UNINSTALL_TARGETS =

DOXYGEN_CLEAN_TARGETS =

DOXYGEN_TARBALLS =

# FIXME: Depending on the source files would be sufficient.
#        Depending on libexif.la works at least, however.
$(top_builddir)/libexif/libexif.la:
	cd $(top_builddir)/libexif && $(MAKE) libexif.la

if BUILD_DOCS
if HAVE_DOXYGEN

DOXYGEN_UPLOAD += $(HTML_APIDOC_DIR).uploaded

DOXYGEN_STAMPS += $(HTML_APIDOC_DIR).stamp
$(HTML_APIDOC_DIR).stamp: Doxyfile $(top_builddir)/libexif/libexif.la
	$(DOXYGEN) $<
	echo > $@

DOXYGEN_TARBALLS += $(HTML_APIDOC_DIR).tar.gz
$(HTML_APIDOC_DIR).tar.gz: $(HTML_APIDOC_DIR).stamp
	(cd $(DOXYGEN_OUTPUT_DIR) && $(AMTAR) chof - $(HTML_APIDOC_DIR) | GZIP=--best gzip -c) > $@

if ENABLE_INTERNAL_DOCS
DOXYGEN_UPLOAD += $(HTML_APIDOC_INTERNALS_DIR).uploaded

DOXYGEN_STAMPS += $(HTML_APIDOC_INTERNALS_DIR).stamp
$(HTML_APIDOC_INTERNALS_DIR).stamp: Doxyfile-internals $(top_builddir)/libexif/libexif.la
	$(DOXYGEN) $<
	echo > $@

DOXYGEN_TARBALLS += $(HTML_APIDOC_INTERNALS_DIR).tar.gz
$(HTML_APIDOC_INTERNALS_DIR).tar.gz: $(HTML_APIDOC_INTERNALS_DIR).stamp
	(cd $(DOXYGEN_OUTPUT_DIR) && $(AMTAR) chof - $(HTML_APIDOC_INTERNALS_DIR) | GZIP=--best gzip -c) > $@

DOXYGEN_INSTALL_TARGETS += install-apidocs-internals
install-apidocs-internals: $(HTML_APIDOC_INTERNALS_DIR).stamp
	$(INSTALL) -m 0755 -d $(DESTDIR)$(docdir)/$(DOC_APIDOC_INTERNALS_HTML)
	$(INSTALL) -m 0644 $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_INTERNALS_DIR)/* $(DESTDIR)$(docdir)/$(DOC_APIDOC_INTERNALS_HTML)/

DOXYGEN_UNINSTALL_TARGETS += uninstall-apidocs-internals
uninstall-apidocs-internals:
	if cd "$(DESTDIR)$(docdir)"; then \
		rm -f "$(DOC_APIDOC_INTERNALS_HTML)/"*; \
		rmdir "$(DOC_APIDOC_INTERNALS_HTML)"; \
	fi;:

endif

# Directory names used while installing
DOC_APIDOC_HTML = api.html
DOC_APIDOC_INTERNALS_HTML = internals.html

DOXYGEN_INSTALL_TARGETS += install-apidocs
install-apidocs: $(HTML_APIDOC_DIR).stamp
	$(INSTALL) -m 0755 -d $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)
	$(INSTALL) -m 0644 $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_DIR)/* $(DESTDIR)$(docdir)/$(DOC_APIDOC_HTML)/

DOXYGEN_UNINSTALL_TARGETS += uninstall-apidocs
uninstall-apidocs:
	if cd "$(DESTDIR)$(docdir)"; then \
		rm -f "$(DOC_APIDOC_HTML)/"*; \
		rmdir "$(DOC_APIDOC_HTML)"; \
	fi;:

DOXYGEN_CLEAN_TARGETS += clean-apidocs
clean-apidocs:
	rm -rf $(DOXYGEN_OUTPUT_DIR)
	rm -f $(HTML_APIDOC_DIR).tar.gz $(HTML_APIDOC_INTERNALS_DIR).tar.gz

$(DOXYGEN_UPLOAD): $(DOXYGEN_STAMPS)
	find $(DOXYGEN_OUTPUT_DIR) \( -type d -exec chmod a+rx,go-w,g+s {} \; \) \
		-or \( -type f -exec chmod a+r {} \; \)
	rsync -avz --progress $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_DIR)/ sf:/home/users/h/hu/hun/libexif/htdocs/api/
if ENABLE_INTERNAL_DOCS
	rsync -avz --progress $(DOXYGEN_OUTPUT_DIR)/$(HTML_APIDOC_INTERNALS_DIR)/ sf:/home/users/h/hu/hun/libexif/htdocs/internals/
endif
	ssh "sf" "find /home/groups/l/li/libexif/htdocs/internals /home/groups/l/li/libexif/htdocs/api \( -type d -exec chgrp libexif {} \; -exec chmod g+rwxs,a+rx {} \; \) -or \( -type f -exec chgrp libexif {} \; -exec chmod g+rw {} \; \( -name '*.html' -or -name '*.png' -or -name '*.php' -or -name '*.jpg' -or -name '*.css' -name '*.dot' -name '*.map' \) -exec chmod a+r {} \; \)"

endif # HAVE_DOXYGEN
endif # BUILD_DOCS


########################################################################
# Common part
########################################################################

EXTRA_DIST = Doxyfile.in Doxyfile-internals.in README.apidocs $(HTML_APIDOC_DIR).tar.gz

doc_DATA = $(DOXYGEN_FILES)

all-local: $(DOXYGEN_STAMPS)

install-data-local: $(DOXYGEN_INSTALL_TARGETS)

uninstall-local: $(DOXYGEN_UNINSTALL_TARGETS)

clean-local: $(DOXYGEN_CLEAN_TARGETS)

upload: $(DOXYGEN_UPLOAD)

CLEANFILES = $(DOXYGEN_FILES) $(DOXYGEN_STAMPS)

