AC_PREREQ(2.59)
AC_INIT([EXIF library],[0.6.13],[libexif-devel@lists.sourceforge.net],[libexif])
AC_CONFIG_SRCDIR([libexif/exif-data.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([gnu 1.8 dist-bzip2])

if test ! -d "$srcdir/m4m"; then
AC_MSG_ERROR([
You are missing the m4m/ directory in your top
$PACKAGE_TARNAME source directory.

You are probably using an ill-maintained CVS tree.
Running

    cd $srcdir
    cvs co m4m

and re-running autogen.sh might help.
])
fi

GP_CHECK_SHELL_ENVIRONMENT
GP_CONFIG_MSG([Build])
GP_CONFIG_MSG([Source code location],[${srcdir}])

dnl ---------------------------------------------------------------------------
dnl Advanced information about versioning:
dnl   * "Writing shared libraries" by Mike Hearn
dnl         http://navi.cx/~mike/writing-shared-libraries.html
dnl   * libtool.info chapter "Versioning"
dnl ---------------------------------------------------------------------------
dnl Versioning:
dnl  - AGE (Micro):      Increment if any interfaces have been added; set to 0
dnl		         if any interfaces have been removed. Removal has 
dnl                      precedence over adding, so set to 0 if both happened.
dnl  - REVISION (Minor): Increment any time the source changes; set to 
dnl			 0 if you incremented CURRENT.
dnl  - CURRENT (Major):  Increment if the interface has changes or removals.
dnl ---------------------------------------------------------------------------
LIBEXIF_AGE=1
LIBEXIF_REVISION=0
LIBEXIF_CURRENT=12
AC_SUBST([LIBEXIF_AGE])
AC_SUBST([LIBEXIF_REVISION])
AC_SUBST([LIBEXIF_CURRENT])
LIBEXIF_VERSION_INFO=`expr $LIBEXIF_CURRENT + $LIBEXIF_REVISION`:$LIBEXIF_AGE:$LIBEXIF_REVISION
AC_SUBST([LIBEXIF_VERSION_INFO])

AC_PROG_CC
AC_C_CONST
dnl FIXME: AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AM_CPPFLAGS="$CPPFLAGS"
GP_CONFIG_MSG([Compiler],[${CC}])


dnl Create a stdint.h-like file containing size-specific integer definitions
dnl that will always be available
AX_NEED_STDINT_H([libexif/_stdint.h])


dnl ---------------------------------------------------------------------------
dnl Whether -lm is required for our math functions
dnl ---------------------------------------------------------------------------

# we need sqrt and pow, but checking for sqrt should be sufficient
AC_ARG_VAR([MATHLIBS],[The libraries required for mathematical functions, e.g. -lm])
if test "x$MATHLIBS" = "x"; then
	# We must not compile with -Wall -Werror here:
	# char *sqrt() conflicts with double sin(const double xx) in any case.
	CFLAGS_save="$CPPFLAGS"
	CPPFLAGS_save="$CPPFLAGS"
	CPPFLAGS=""
	CFLAGS=""
	AC_CHECK_FUNC([sqrt],[
		MATHLIBS=""
	],[
		AC_CHECK_LIB([m],[sqrt],[
			MATHLIBS="-lm"
		],[
			AC_MSG_ERROR([
*** Could not find sqrt() function
])
		])
	])
	CFLAGS="$CFLAGS_save"
	CPPFLAGS="$CPPFLAGS_save"
fi
AC_SUBST([MATHLIBS])


# doc support
GP_CHECK_DOC_DIR
GP_CHECK_DOXYGEN


# ---------------------------------------------------------------------------
# i18n support
# ---------------------------------------------------------------------------
GP_GETTEXT_HACK([${PACKAGE}-${LIBEXIF_CURRENT}],
                [Lutz Müller and others])
ALL_LINGUAS="de es fr"
AM_GNU_GETTEXT_VERSION([0.14.1])
AM_GNU_GETTEXT([external])
AM_PO_SUBDIRS()
AM_ICONV()
GP_GETTEXT_FLAGS()

dnl We cannot use AC_DEFINE_UNQUOTED() for these definitions, as
dnl we require make to do insert the proper $(datadir) value
localedir="\$(datadir)/locale"
AC_SUBST(localedir)
AM_CPPFLAGS="$AM_CPPFLAGS -DLOCALEDIR=\\\"${localedir}\\\""


dnl ---------------------------------------------------------------------------
dnl Compiler/Linker Options and Warnings
dnl ---------------------------------------------------------------------------
AM_CPPFLAGS="$AM_CPPFLAGS -I\$(top_srcdir)"
AM_CPPFLAGS="$AM_CPPFLAGS -I\$(top_srcdir)/libexif"
AM_LDFLAGS="$LDFLAGS"
if test "x$GCC" = "xyes"; then
    AM_CPPFLAGS="$AM_CPPFLAGS -g -Wall -Wmissing-declarations -Wmissing-prototypes"
    AM_LDFLAGS="$AM_LDFLAGS -g -Wall"
fi



AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)


dnl ---------------------------------------------------------------------------
dnl Output files
dnl ---------------------------------------------------------------------------
AC_CONFIG_FILES([ 
  po/Makefile.in
  Makefile
  libexif.spec
  libexif/Makefile
  libexif/canon/Makefile
  libexif/olympus/Makefile
  libexif/pentax/Makefile
  test/Makefile
  test/nls/Makefile
  m4m/Makefile
  doc/Makefile
  doc/Doxyfile
  libexif/libexif.pc
])
AC_OUTPUT

GP_CONFIG_OUTPUT
