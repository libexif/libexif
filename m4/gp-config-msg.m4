dnl
dnl GP_CONFIG_MSG
dnl
dnl Simple way to print a configuration summary at the end of ./configure.
dnl
dnl Example usage:
dnl
dnl    GP_CONFIG_INIT
dnl    GP_CONFIG_MSG([Source code location],[${srcdir}])
dnl    GP_CONFIG_MSG([Compiler],[${CC}])
dnl    GP_CONFIG_MSG
dnl    GP_CONFIG_MSG([Feature foo],[${foo}])
dnl    GP_CONFIG_MSG([Location of bar],[${bar}])
dnl    GP_CONFIG_OUTPUT
dnl
dnl
AC_DEFUN([GP_CONFIG_INIT],
[# the empty string must contain at least as many spaces as the substr length
ndim_config_empty="                        "
ndim_config_msg="
Configuration (${PACKAGE_TARNAME} ${PACKAGE_VERSION}):

";])dnl
dnl
AC_DEFUN([GP_CONFIG_MSG],
[AC_REQUIRE([GP_CONFIG_INIT])dnl
m4_if([$1],,[
# Empty line in config message output
ndim_config_msg="${ndim_config_msg}
"
],[
ndim_config_msg="${ndim_config_msg}	$(expr "$1:${ndim_config_empty}" : "\(.\{0,22\}\)") $2
"
])])dnl
dnl
AC_DEFUN([GP_CONFIG_OUTPUT],
[AC_REQUIRE([GP_CONFIG_INIT])dnl
AC_REQUIRE([GP_CONFIG_MSG])dnl
echo "${ndim_config_msg}
You may run \"make\" and \"make install\" now.";])dnl
dnl
dnl Please do not remove this:
dnl filetype: de774af3-dc3b-4b1d-b6f2-4aca35d3da16
dnl I use this to find all the different instances of this file which 
dnl are supposed to be synchronized.
dnl
dnl Local Variables:
dnl mode: autoconf
dnl End:
