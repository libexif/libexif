SUBDIRS = libexif libjpeg test po intl

CONFIG_CLEAN_FILES =
EXTRA_DIST = @PACKAGE@.spec


########################################################################
# create .tar.bz2 distribution package for convenience if the
# $(TAR) on this system supports it. else do nothing.
dist-hook:
	if [ "x${CDPATH}" != "x" ]; then \
		echo "You have set CDPATH. If make dist fails, try to unset CDPATH."; \
	fi
	-$(MAKE) dist-bzip2

dist-bzip2:
	-chmod -R a+r $(distdir)
	-$(TAR) chojf $(distdir).tar.bz2 $(distdir)
