dnl
dnl GP_GETTEXT_HACK
dnl
dnl gphoto2 gettext hack, designed for libexif, libgphoto2, and Co.
dnl
dnl Example usage:
dnl    GETTEXT_PACKAGE=${PACKAGE}-${LIBEXIF_CURRENT}
dnl    GP_GETTEXT_HACK
dnl    AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],
dnl                       [The gettext domain we're using])
dnl    AC_SUBST([GETTEXT_PACKAGE])
dnl    ALL_LINGUAS="de es fr"
dnl    AM_GNU_GETTEXT
dnl

AC_DEFUN([GP_GETTEXT_HACK],
[
if test "x$GETTEXT_PACKAGE" = "x"; then
   AC_MSG_ERROR([
*** Your configure.{ac,in} is wrong.
*** GETTEXT_PACKAGE must be defined before calling [GP_GETTEXT_HACK].
***
])
fi
sed_cmds="s|^DOMAIN.*|DOMAIN = ${GETTEXT_PACKAGE}|"
if test -n "$1"; then
   sed_cmds="${sed_cmds};s|^COPYRIGHT_HOLDER.*|COPYRIGHT_HOLDER = $1|"
fi
if test -n "$2"; then
   sed_mb="$2"
elif test -n "$PACKAGE_BUGREPORT"; then
   sed_mb="${PACKAGE_BUGREPORT}"
else
   AC_MSG_ERROR([
*** Your configure.{ac,in} is wrong.
*** Either define PACKAGE_BUGREPORT (by using the 4-parameter AC INIT syntax)
*** or give [GP_GETTEXT_HACK] the second parameter.
***
])
fi
sed_cmds="${sed_cmds};s|^MSGID_BUGS_ADDRESS.*|MSGID_BUGS_ADDRESS = ${sed_mb}|"
# Not so sure whether this hack is all *that* evil...
AC_MSG_CHECKING([for po/Makevars requiring hack])
if test -f po/Makevars.template; then
   sed "$sed_cmds" < po/Makevars.template > po/Makevars
   AC_MSG_RESULT([yes, done.])
else
   AC_MSG_RESULT([no])
fi
])

dnl Local Variables:
dnl mode: autoconf
dnl End:
