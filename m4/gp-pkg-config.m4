dnl @synopsis GP_PKG_CONFIG
dnl
dnl If you want to set the PKG_CONFIG_PATH, best do so before
dnl calling GP_PKG_CONFIG
AC_DEFUN([GP_PKG_CONFIG],[
AC_ARG_VAR([PKG_CONFIG_PATH],[where pkg-config looks for *.pc files])
export PKG_CONFIG_PATH

AC_MSG_CHECKING([PKG_CONFIG_PATH])
if test "x${PKG_CONFIG_PATH}" = "x"; then
	AC_MSG_RESULT([empty])
else
	AC_MSG_RESULT([${PKG_CONFIG_PATH}])
fi

AC_PATH_PROG([PKG_CONFIG],[pkg-config])
])dnl

dnl Please do not remove this:
dnl filetype: d87b877b-80ec-447c-b042-21ec4a27c6f0
dnl I use this to find all the different instances of this file which 
dnl are supposed to be synchronized.

dnl Local Variables:
dnl mode: autoconf
dnl End:
