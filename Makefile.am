SUBDIRS = m4 po libexif libjpeg test

EXTRA_DIST = @PACKAGE_TARNAME@.spec

ACLOCAL_AMFLAGS = -I m4

#######################################################################
# Help for the maintainer
#

# Simulate something like
#   EXTRA_DIST_IF_EXIST = ChangeLog.cvs
# If present, ship ChangeLog.cvs in source tarball.
# If not present, don't ship it.
dist-hook:
	if test -f $(srcdir)/ChangeLog.cvs; then \
		cp -p $(srcdir)/ChangeLog.cvs $(distdir)/ChangeLog.cvs; \
	fi

.PHONY: cvs-changelog
cvs-changelog: $(srcdir)/ChangeLog.cvs

.PHONY: $(srcdir)/ChangeLog.cvs
$(srcdir)/ChangeLog.cvs:
	if test -f "$(srcdir)/cvs2cl.usermap"; then \
		usermap="--usermap ./cvs2cl.usermap"; \
	else \
		usermap=""; \
	fi; \
	(cd "$(srcdir)" && cvs2cl $${usermap} -f "ChangeLog.cvs")
	
.PHONY: cvs-tag-release
cvs-tag-release:
	@tag="$$(echo "$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-release" | sed 's|\.|_|g')"; \
	echo "Are you sure you want to CVS tag \`$${tag}' your source files?"; \
	echo "Press Ctrl-C to abort, Enter to continue."; \
	read; \
	cd "$(srcdir)" && cvs tag "$${tag}"

# End of Makefile.am.
