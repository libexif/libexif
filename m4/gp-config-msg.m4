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
