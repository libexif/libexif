dnl @synopsis GP_CHECK_LIBRARY([VAR_PREFIX],[libname],[>= version],[optional])
dnl
dnl Examples:
dnl    GP_CHECK_LIBRARY([LIBEXIF],[libexif])dnl
dnl    GP_CHECK_LIBRARY([LIBEXIF],[libexif-gtk], [>= 0.3.3])dnl
dnl    GP_CHECK_LIBRARY([LIBEXIF],[libexif-gtk], [>= 0.3.3], [optional])dnl
dnl    GP_CHECK_LIBRARY([LIBEXIF],[libexif], [], [mandatory])dnl
dnl
dnl By default, the library is mandatory.
dnl
dnl Things to add:
dnl   * --with-libfoo=autodetect
dnl   * --with-libfoo-include-dir=/foo
dnl   * --with-libfoo-prefix=/usr/local
dnl   * --with-libfoo-lib=/usr/local/lib64
dnl
AC_DEFUN([_GP_CHECK_LIBRARY],[
dnl
AC_REQUIRE([GP_CONFIG_MSG])dnl
dnl
AC_ARG_VAR([$1][_CFLAGS], [CFLAGS for compiling with ][$2])dnl
AC_ARG_VAR([$1][_LIBS],   [libs to add for linking against ][$2])dnl
dnl
AC_MSG_CHECKING([which ][$2][ to use])
if test "x${[$1][_LIBS]}" = "x" && test "x${[$1][_CFLAGS]}" = "x"; then
	AC_MSG_RESULT([autodetect])
	PKG_CHECK_MODULES([$1], [$2][ $3], [], [dnl
		m4_if([$4],[mandatory],[AC_MSG_ERROR([
* Fatal: ${PACKAGE_NAME} requires $2 to build.
])],[$4],[optional],[])])
elif test "x${[$1][_LIBS]}" != "x" && test "x${[$1][_CFLAGS]}" != "x"; then
	AC_MSG_RESULT([user-defined])
else
	AC_MSG_ERROR([
* Either set [$1][_LIBS] *and* [$1][_CFLAGS] or none at all
* when calling configure for the ${PACKAGE_NAME}.
])
fi
dnl AC_SUBST is done implicitly by AC_ARG_VAR :-)
dnl AC_SUBST([$1][_LIBS])
dnl AC_SUBST([$1][_CFLAGS])
GP_CONFIG_MSG([$2][ library],[${[$1][_LIBS]}])dnl
GP_CONFIG_MSG([$2][ cflags], [${[$1][_CFLAGS]}])dnl
])dnl
dnl
dnl ####################################################################
dnl
AC_DEFUN([__GP_CHECK_LIBRARY], [dnl
m4_if([$4],[],[_GP_CHECK_LIBRARY([$1],[$2],[$3],[mandatory])],
      [_GP_CHECK_LIBRARY([$1],[$2],[$3],[$4])])dnl
])dnl
dnl
dnl ####################################################################
dnl
AC_DEFUN([GP_CHECK_LIBRARY],[dnl
m4_if([$#], 2,[
# ----------------------------------------------------------------------
# $0([$1],[$2])
# ----------------------------------------------------------------------
__GP_CHECK_LIBRARY([$1],[$2])], [$#], 3,[
# ----------------------------------------------------------------------
# $0([$1],[$2],[$3])
# ----------------------------------------------------------------------
__GP_CHECK_LIBRARY([$1],[$2],[$3])], 4,[
# ----------------------------------------------------------------------
# $0([$1],[$2],[$3],[$4])
# ----------------------------------------------------------------------
__GP_CHECK_LIBRARY([$1],[$2],[$3],[$4])], [
m4_errprint([Illegal number of arguments ($#) to $0 macro
])m4_exit(1)])dnl
])dnl
dnl
dnl Please do not remove this:
dnl filetype: 6e60b4f0-acb2-4cd5-8258-42014f92bd2c
dnl I use this to find all the different instances of this file which 
dnl are supposed to be synchronized.
dnl
dnl Local Variables:
dnl mode: autoconf
dnl End:
