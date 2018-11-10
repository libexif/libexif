dnl Search for libfailmalloc to use for testing
AC_DEFUN([CHECK_FAILMALLOC],[dnl
  dnl Libtool sets the default library paths
  AM_PROG_LIBTOOL
  path_provided=
  AC_ARG_WITH(failmalloc, [  --with-failmalloc=PATH  use Failmalloc for tests], [
      if test x"$withval" = "x" -o x"$withval" = "xyes"; then
        failmalloc_search_path="$sys_lib_search_path_spec"
      elif test x"$withval" = "xno"; then
        failmalloc_search_path=""
      else
        failmalloc_search_path="$withval"
        path_provided=1
      fi
    ], [failmalloc_search_path="$sys_lib_search_path_spec"]
  )
  libfailmalloc_file=libfailmalloc.so.0
  FAILMALLOC_PATH=
  dnl Check if the argument is a directory
  for d in $failmalloc_search_path; do
      AC_CHECK_FILE([$d/$libfailmalloc_file], [
        FAILMALLOC_PATH="$d/$libfailmalloc_file"
        break
      ], [])
  done
  if test -z "$FAILMALLOC_PATH" -a -n "$path_provided"; then
    dnl Check if the argument is a file
    AC_CHECK_FILE([$failmalloc_search_path], [FAILMALLOC_PATH="$failmalloc_search_path"], [])
  fi
  AC_MSG_CHECKING([for failmalloc])
  dnl Make sure AC_CHECK_FILE didn't find a directory by mistake
  if test -n "$FAILMALLOC_PATH" -a -f "$FAILMALLOC_PATH"; then
    AC_MSG_RESULT([yes])
  else
    if test -n "$path_provided"; then
      AC_MSG_ERROR([$libfailmalloc_file was not found at $failmalloc_search_path])
    else
      AC_MSG_RESULT([no])
    fi
  fi
  AC_SUBST(FAILMALLOC_PATH)
  AM_CONDITIONAL(USE_FAILMALLOC, [test "x$FAILMALLOC_PATH" != "x"])
])

