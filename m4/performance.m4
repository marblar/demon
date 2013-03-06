# https://github.com/kr/cubby/blob/master/m4/gcov.m4
dnl Profide a configure option to enable gcov compiler flags.
dnl Derived from gcov.m4 in the Gobject-Introspection repository.
AC_DEFUN([ML_PERFORMANCE_TEST],
[
  AC_ARG_ENABLE(performance_tests,
    AC_HELP_STRING([--enable-performance-tests],
      [Adds performance test suite to test suite.]),
    enable_performance_tests=$enableval,
    enable_performance_tests=no)
  if test x$enable_perftest = xyes
  then
    dnl libtool 1.5.22 and lower strip -fprofile-arcs from the flags
    dnl passed to the linker, which is a bug; -fprofile-arcs implicitly
    dnl links in -lgcov, so we do it explicitly here for the same effect

    PERFORMANCE_TESTS_ENABLED=yes
    AC_DEFINE_UNQUOTED(PERFORMANCE_TESTS_ENABLED, 1,
      [Defined if performance tests are enabled.])

    # Force the user to turn off optimization
    AC_MSG_NOTICE([Performance tests enabled, adding "-g -O3" to CFLAGS])
    AS_COMPILER_FLAGS(CXXFLAGS, "-g -O3")
  fi
  AM_CONDITIONAL(PERFORMANCE_TESTS_ENABLED, test x$enable_performance_tests = xyes)
])
