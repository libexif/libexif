dnl
dnl GP_CONFIG_MSG
dnl
dnl Simple way to print a configuration summary at the end of ./configure.
dnl
dnl Example usage:
dnl    
dnl    GP_CONFIG_MSG([Source code location],[${srcdir}])
dnl    GP_CONFIG_MSG([Compiler],[${CC}])
dnl    GP_CONFIG_MSG
dnl    GP_CONFIG_MSG([Feature foo],[${foo}])
dnl    GP_CONFIG_MSG([Location of bar],[${bar}])
dnl    GP_CONFIG_OUTPUT
dnl

AC_DEFUN([GP_CONFIG_MSG],
[
# the empty string must contain at least as many spaces as the substr length
ndim_config_empty="                        "
if test -n "$1"; then
   ndim_config_msg="${ndim_config_msg}	$(expr substr "$1:${ndim_config_empty}" 1 22) $2
"
else
   ndim_config_msg="${ndim_config_msg}
"
fi
])

AC_DEFUN([GP_CONFIG_OUTPUT],
[
AC_REQUIRE([GP_CONFIG_MSG])
echo "

Configuration (${PACKAGE_TARNAME}):

${ndim_config_msg}"
])
